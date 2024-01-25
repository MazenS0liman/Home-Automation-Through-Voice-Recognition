import speech_recognition as sr
import serial
import time
import pygame

# Serial port configuration
ser = serial.Serial('/dev/tty.usbmodem11201', 9600, timeout=1)


def play_sound(file_path):
    pygame.init()
    pygame.mixer.init()

    try:
        pygame.mixer.music.load(file_path)
        pygame.mixer.music.play()

        # Allow time for the sound to play
        while pygame.mixer.music.get_busy():
            pygame.time.Clock().tick(10)

    except Exception as e:
        print(f"Error: {e}")

    finally:
        pygame.mixer.quit()

def detect_sound():
     # Initialize the recognizer
    recognizer = sr.Recognizer()
    with sr.Microphone() as source:
        try:    
            print("Say something:")
            audio = recognizer.listen(source, timeout=5)
           

            try:
                # Recognize speech using Google Web Speech API
                text = recognizer.recognize_google(audio)
                print(f"You said: {text}")

                #Send the command to the Pico
                if text=="good morning" or text=="goodmorning":
                    ser.write(b'1')
                    time.sleep(0.1) 
                    return text
                    
                if text=="good night" or text=="goodnight":
                    ser.write(b'0')
                    time.sleep(0.1)
                    return text 
                    
                return text 
                    
            except sr.UnknownValueError:
                print("Sorry, I could not understand what you said.")
            except sr.RequestError as e:
                print(f"Error making the request to Google Web Speech API: {e}")
        except sr.WaitTimeoutError:
            print("no input audio.")
        



# Use the default microphone as the audio source
while True:
    text=detect_sound()

    if text=="good morning" or text=="goodmorning":
        play_sound('rooster.wav')
        print("sound")
    
    time.sleep(1)
    # text=detect_sound()
    if text=="good night" or text=="goodnight":
        play_sound('Animation.wav')
    