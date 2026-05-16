import serial
import csv
import time
import os

PORT = '/dev/cu.usbmodem14301'
BAUD = 115200
OUTPUT_DIR = 'gesture_data'

os.makedirs(OUTPUT_DIR, exist_ok=True)

def collect_sample(ser, label, sample_num):
    ser.reset_input_buffer()
    ser.write(b'RECORD\n')
    
    while True:
        line = ser.readline().decode('utf-8', errors='ignore').strip()
        if line == 'START':
            break
    
    rows = []
    while len(rows) < 100:
        line = ser.readline().decode('utf-8', errors='ignore').strip()
        if line == 'END':
            break
        if line and ',' in line:
            rows.append(line.split(','))
    
    filename = f'{OUTPUT_DIR}/{label}_{sample_num:03d}.csv'
    with open(filename, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(['ax', 'ay', 'az', 'gx', 'gy', 'gz'])
        writer.writerows(rows)
    
    return len(rows) == 100

def main():
    label = input('Gesture label (shake/wave/idle): ').strip()
    batch_size = int(input('Batch size (e.g. 10 = 10 samples in a row): ') or '10')
    
    ser = serial.Serial(PORT, BAUD, timeout=5)
    time.sleep(2)
    
    # 이미 있는 샘플 개수 확인 (이어서 모으기)
    existing = [f for f in os.listdir(OUTPUT_DIR) if f.startswith(f'{label}_')]
    sample_num = len(existing)
    print(f'\n"{label}" already has {sample_num} samples.')
    
    while True:
        cmd = input(f'\n[{sample_num} total] Press Enter to start batch of {batch_size}, "q" to quit: ')
        if cmd.lower() == 'q':
            break
        
        for i in range(batch_size):
            print(f'  [{i+1}/{batch_size}] Recording in 1s... GO!', end=' ', flush=True)
            time.sleep(1)
            
            if collect_sample(ser, label, sample_num):
                print(f'OK ({sample_num+1})')
                sample_num += 1
            else:
                print('FAILED, retry')
            
            time.sleep(0.5)  # 다음 샘플 전 잠깐 쉼
    
    ser.close()
    print(f'\nDone. Total "{label}": {sample_num}')

if __name__ == '__main__':
    main()