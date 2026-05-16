import pickle
from micromlgen import port

with open('gesture_model.pkl', 'rb') as f:
    model = pickle.load(f)

c_code = port(model, classmap={0: 'shake', 1: 'wave', 2: 'idle'})

with open('gesture_model.h', 'w') as f:
    f.write(c_code)

print('Done! gesture_model.h saved.')