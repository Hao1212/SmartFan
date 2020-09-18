import blynklib
import blynktimer
import network

BLYNK_AUTH = #auth token
WIFI_SSID = #'YourWifiSSID'
WIFI_PASS = #'YourWifiPassword'
blynk = blynklib.Blynk(BLYNK_AUTH)
timer = blynktimer.Timer()
TEMP_PRINT_MSG = "[WRITE_VIRTUAL_WRITE] Pin: V{} Value: '{}'"
T_COLOR = '#f5b041' # temperature color on blynk
ERR_COLOR = '#444444'
temp_threshold = 26 # to be set or read from blynk

#Wifi connection
print("Connecting to WiFi network '{}'".format(WIFI_SSID))
wifi = network.WLAN(network.STA_IF)
wifi.active(True)

wifi.connect(WIFI_SSID, WIFI_PASS)
# check if board connected 
connect_status = wifi.isconnected()
while not connect_status:
    time.sleep(1)
    print('WiFi connect retry ...')
print('WiFi IP:', wifi.ifconfig()[0])

#get temperature from sensor, write to vpin3
T_VPIN = 3 # bind vpin 3 in blynk app
dht22 = dht.DHT22(Pin(4, Pin.IN, Pin.PULL_UP)) #to be changed to our own sensor 

@blynk.handle_event('read V{}'.format(T_VPIN))
def read_handler(vpin):
    temperature = -999
    # read sensor data
    try:
        dht22.measure()
        temperature = dht22.temperature()#can it be read outside this function?
    except OSError as o_err:
        print("Unable to get DHT22 sensor data: '{}'".format(o_err))
    # change widget values and colors according read results
    if temperature != -999 :
        blynk.set_property(T_VPIN, 'color', T_COLOR)
        blynk.virtual_write(T_VPIN, temperature)
    else:
    # show widgets aka 'disabled' that mean we had errors during read sensor operation
        blynk.set_property(T_VPIN, 'color', ERR_COLOR)
    
    #at this point we got temp & wrote to vpin
    #if-else logic, we can set mutiple threshold here [todo]
    if temperature >= temp_threshold:
        #run the fan![todo]
        blynk.notify('dont u want cooooool')
        #we can also modify color showed on blynk here.[todo]
    

@timer.register(vpin_num=3, interval=10, run_once=False)
def write_to_virtual_pin(vpin_num=1):# why 1?
    print(TEMP_PRINT_MSG.format(vpin_num, temperature))
    blynk.virtual_write(vpin_num, temperature)


while True:
    blynk.run()
    timer.run()