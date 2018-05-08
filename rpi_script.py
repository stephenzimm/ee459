from block_io import BlockIo
import json
import serial
import time
import datetime

version = 2 #API version
api_Key = 'e2f5-1de5-04ef-606c'
secret_pin = 'uscee45916'
block_io = BlockIo(api_Key, secret_pin, version)

ser = serial.Serial("/dev/ttyAMA0",9600,timeout=1)

#Make sure the program autostarts
fo = open('starts.txt', 'wb')
fo.write('< -------  starts --------- > \n ')
ts = time.time()
st = datetime.datetime.fromtimestamp(ts).strftime('%Y-%m-%d %H:%M:%S')
fo.write(st)
fo.close()

#Send Coin
def sendCoin( address, amount ):
    print('test '  + str(amount))
    block_io.withdraw(amounts=amount, to_addresses=address)
    #print('success')
    print(block_io)
    return;

#Check Balance
def checkBalance():
    json_string = block_io.get_balance()
    #print(json_string)
    js = json.dumps(json_string)
    parsed_js = json.loads(js)
    data = parsed_js['data']
    balance = data['available_balance']
    print('available balance: ' + str(balance))
    #print('success')
    
    return balance

#Serial In
def serialIn():
    #ser.open()
    sIn = ''
    while True:
        sIn = ser.readline()
        print(sIn)
        print(len(sIn))
#       print(ser.readline())
        ser.flush()
        if (len(sIn) > 0):
            #ser.close()
            print('here')
            break
    return sIn;

#Serial Out
def serialOut( outPut ):
    ser.write(outPut)
    ser.flush()
    #time.sleep(1)
    #return;

    
#main
#Current Rate : BTC(TEST)/USD : 200.1
RATE = 200.1
while(1):
    cmd = serialIn()
    print('command : ' + cmd)
    if (cmd == 's'):
        print('Papare to SEND')
        amountStr = serialIn();
        amount = float(amountStr[:-1])/RATE
        amount = round(amount,2)
        print('amount : ' + amountStr[:-1])
        addr = serialIn()
        print('address: ' + addr)
        sendCoin(addr, amount)
        print('DONE SENDING')
        #serialOut('SUCCESS')
        
    elif (cmd == 'c'):
        print('Check Balance')
        balance = checkBalance()
        #balance = round(float(balance), 3)
        print('DONE CHECKING')
        balanceStr = str(balance)
        
        #eth amount
        for char in balanceStr:
            print('char to send: ' + char)
            serialOut(char)
            time.sleep(0.4)
        print('DONE CHECKING')


