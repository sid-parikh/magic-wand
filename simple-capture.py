#!/usr/bin/env python3
import serial
import json
import argparse
import time
import sys
import os

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('port')
    parser.add_argument('output_file')
    args = parser.parse_args()
    
    try:
        ser = serial.Serial(args.port, 115200, timeout=1)
        time.sleep(2)
        ser.reset_input_buffer()
        
        all_samples = []
        
        print("capturing 100 samples")
        
        capture_count = 0
        while capture_count < 100:
            # Countdown with beep
            for count in [3, 2, 1]:
                time.sleep(0.5)
                print(f"{count}...", end="", flush=True)
                sys.stdout.write('\a')
                sys.stdout.flush()
            
            print("Capture!")
            ser.write(b'\n')
            
            current_samples = []
            sample_count = 0
            
            print("Collecting...")
            
            while sample_count < 50:
                line = ser.readline().decode('utf-8').strip()

                if not line.startswith("S") and not line.startswith("C") and line:
                    print(line)
                    parts = line.split(" ")
                    
                    current_samples.append({
                        "aX": float(parts[1]),
                        "aY": float(parts[2]),
                        "aZ": float(parts[3]),
                        "gX": float(parts[4]),
                        "gY": float(parts[5]),
                        "gZ": float(parts[6]),
                    })

                    sample_count += 1
            
            all_samples.append(current_samples)
            capture_count += 1
            print(f"sample collected! {capture_count}/100 complete...")
            time.sleep(0.5)

    except serial.SerialException as e:
        print(f"Error: {e}")
    except KeyboardInterrupt:
        print("\nTerminated")
    finally:
        ser.close()

        with open(f"output/{args.output_file}", 'w') as f:
            json.dump({"samples": all_samples}, f)

        print(f"Saved to {args.output_file}")

if __name__ == "__main__":
    main()