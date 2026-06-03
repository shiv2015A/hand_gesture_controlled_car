import cv2
import serial
import cvzone
from cvzone.HandTrackingModule import HandDetector
import time

def main():
    # Attempt to initialize Serial Communication
    # Ensure Arduino is connected to COM4, or change the port accordingly.
    try:
        ser = serial.Serial("COM4", 9600, timeout=1)
        print("[INFO] Successfully connected to Arduino on COM4")
        time.sleep(2) # Give Arduino time to reset on connection
    except serial.SerialException:
        print("[WARNING] Could not connect to COM4. Serial commands won't be sent.")
        ser = None

    # Initialize Webcam
    cap = cv2.VideoCapture(0)
    if not cap.isOpened():
        print("[ERROR] Could not open webcam.")
        return

    # Initialize Hand Detector
    # maxHands=1: Only track one hand for clear car instructions.
    # detectionCon=0.8: High confidence threshold to avoid false positives.
    detector = HandDetector(maxHands=1, detectionCon=0.8)

    print("[INFO] Starting video stream... Press ESC to exit.")
    
    while True:
        success, img = cap.read()
        if not success:
            print("[ERROR] Failed to grab frame.")
            break
            
        # Flip the image horizontally for natural mirroring
        img = cv2.flip(img, 1)

        # Detect hands and overlay landmarks
        hands, img = detector.findHands(img, draw=True)

        if hands:
            # We are only detecting max 1 hand, grab the first one
            hand = hands[0]
            
            # fingersUp() returns a list of 5 boolean/integer values [Thumb, Index, Middle, Ring, Pinky]
            # 1 indicates the finger is up, 0 indicates it is down
            fingers = detector.fingersUp(hand)
            
            command = b''
            
            # Hand Gesture Mapping per documentation
            if fingers == [0, 0, 0, 0, 0]:
                print("Command: back")
                command = b'b'
            elif fingers == [0, 1, 0, 0, 0]:
                print("Command: FORWARD")
                command = b'f'
            elif fingers == [1, 0, 0, 0, 0]:
                print("Command: left")
                command = b'L'
            elif fingers == [0, 0, 0, 0, 1]:
                print("Command: right")
                command = b'R'
            elif fingers == [1, 1, 1, 1, 1]:
                print("Command: stop (open hand)")
                command = b's'
            else:
                # If gesture is unrecognized, default to stop for safety
                print("Command: Unrecognized gesture -> stop")
                command = b's'
            
            # Dispatch command safely
            if ser and command:
                ser.write(command)

        else:
            # No hands detected -> Fail-safe Stop
            print("Command: No hands detected - stopping actions")
            if ser:
                ser.write(b's')

        # Display the Processed Frame
        cv2.imshow("Hand Gesture Control - Car", img)

        # Poll keyboard. ASCII 27 is the Escape key.
        if cv2.waitKey(1) & 0xFF == 27:
            print("[INFO] Exiting program...")
            break

    # Release Resources gracefully
    if ser:
        ser.write(b's') # Final stop command before closing
        ser.close()
    cap.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
