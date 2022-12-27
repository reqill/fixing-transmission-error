import random

masks = [
    [True]*8,
    [False,True]*4,
    [False,False,True,True]*2,
    [False,False,False,False,True,True,True,True]
]

def checkParity(byte,pow):
    global masks
    sum = 0
    for i in range(8):
        if masks[pow][i]:
            sum += byte[i]
    return sum%2

def encode(byte):
    byte1 = [
        0,
        0,
        0,
        byte[0],
        0,
        byte[1],
        byte[2],
        byte[3]
    ]
    byte2 = [
        0,
        0,
        0,
        byte[4],
        0,
        byte[5],
        byte[6],
        byte[7]
    ]
    byte1[4] = checkParity(byte1,3)
    byte1[2] = checkParity(byte1,2)
    byte1[1] = checkParity(byte1,1)
    byte1[0] = checkParity(byte1,0)
    byte2[4] = checkParity(byte2,3)
    byte2[2] = checkParity(byte2,2)
    byte2[1] = checkParity(byte2,1)
    byte2[0] = checkParity(byte2,0)
    return byte1,byte2

def messUp(byte):
    idx =random.randint(0,7)
    byte[idx] = (byte[idx]+1)%2




def fix(byte):
    global masks
    error = [True]*8
    for pow,mask in enumerate(masks):
        if checkParity(byte,pow):
            for x in range(8):
                error[x] = error[x] and mask[x]
        else:
            for x in range(8):
                error[x] = error[x] and (not mask[x])
    
    for idx in range(8):
        if error[idx]:
            byte[idx] = (byte[idx] +1)%2
    
def decode(b1,b2):
    return [b1[3],b1[5],b1[6],b1[7],b2[3],b2[5],b2[6],b2[7]]

if _name_ == "_main_":

    for _ in range(1000):
        byte = [random.randint(0,1) for _ in range(8)]

        b1,b2 = encode(byte)
        
        messUp(b1)
        messUp(b2)
    
        fix(b1)
        fix(b2)
    
        res = decode(b1,b2)
        if res != byte:
            print("ERROR")
            break
    else:
        print("All good")