import numpy as np
import pandas as pd
import os
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import train_test_split
from sklearn.metrics import classification_report, confusion_matrix
import pickle

DATA_DIR = 'gesture_data'
LABELS = ['shake', 'wave', 'idle']
SAMPLES_PER_FILE = 100

def extract_features(df):
    features = []
    for col in ['ax', 'ay', 'az', 'gx', 'gy', 'gz']:
        features.append(df[col].mean())
        features.append(df[col].std())
        features.append(df[col].max())
        features.append(df[col].min())
        features.append(df[col].abs().max())
    return features

def load_data():
    X, y = [], []
    for label_idx, label in enumerate(LABELS):
        files = [f for f in os.listdir(DATA_DIR) if f.startswith(f'{label}_')]
        print(f'{label}: {len(files)} samples')
        for fname in files:
            df = pd.read_csv(f'{DATA_DIR}/{fname}')
            if len(df) == SAMPLES_PER_FILE:
                X.append(extract_features(df))
                y.append(label_idx)
    return np.array(X), np.array(y)

# 데이터 로드
print('Loading data...')
X, y = load_data()
print(f'Total: {len(X)} samples, {X.shape[1]} features each')

# 학습/테스트 분리 (70/15/15)
X_train, X_temp, y_train, y_temp = train_test_split(X, y, test_size=0.3, random_state=42, stratify=y)
X_val, X_test, y_val, y_test = train_test_split(X_temp, y_temp, test_size=0.5, random_state=42, stratify=y_temp)

print(f'\nTrain: {len(X_train)}, Val: {len(X_val)}, Test: {len(X_test)}')

# 모델 학습
print('\nTraining Random Forest...')
model = RandomForestClassifier(n_estimators=10, random_state=42)
model.fit(X_train, y_train)

# 평가
val_acc = model.score(X_val, y_val)
test_acc = model.score(X_test, y_test)
print(f'\nVal accuracy:  {val_acc:.3f}')
print(f'Test accuracy: {test_acc:.3f}')

print('\nClassification Report:')
print(classification_report(y_test, model.predict(X_test), target_names=LABELS))

print('Confusion Matrix:')
print(confusion_matrix(y_test, model.predict(X_test)))

# 모델 저장
with open('gesture_model.pkl', 'wb') as f:
    pickle.dump(model, f)
print('\nModel saved: gesture_model.pkl')