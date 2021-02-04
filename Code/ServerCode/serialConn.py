import serial
from serial import Serial
import re
import requests
import json
import time
import sys


ACCESS_TOKEN = 'ya29.a0AfH6SMDgXrHR76m6CAJGmeGPTJvVtMd5GZHSTJj39-pcBkkDXjprOfgQiQj6NwU_FGfJyGGh8oKUntjuHKe9Qh9rNM_qFc1G_BOus2EW4KLUTKHp6trHFhm6DZeq7xJkNnWYXWf51kjMLarockLdKFqpKfjg7R8xSx2OJEIPKQga'

HEADERS = {
	  'Content-Type': 'application/json',
	  'Authorization': 'Bearer {}'.format(ACCESS_TOKEN),
  }
  
def Get_Temp_From_Atmega():
	'''
	params: None

	Listen to the atmega for values, if there is no change it will return None else
	it will return the temperature value.

	return: the Temperature difference reading from ATMEGA

	'''
	ser = serial.Serial('/dev/tty.usbserial-AC01N8HZ',baudrate=9600,timeout=1000)
	readings_list = []
	while True:
		s = ser.readline()
		strg = s.decode('utf-8')
		if 'NODATA' in strg:
			ser.close()
			return  
		try:

			readings_list.append(float(re.findall("\d+\.\d+",strg)[0]))
			ser.close()
			return readings_list[0]
		except Exception as e:
			pass
			
def getCurrentTemp():
	'''
	params: None

	Send a GET request to the NEST API endpoint to retrieve the current Temperature.

	return: the current Temperature reading in the google nest in degree celcius

	'''
	response = requests.get('https://smartdevicemanagement.googleapis.com/v1/enterprises/88e358d5-7a5d-42d1-aeb8-a487f595fa89/devices', headers=HEADERS)
	data = json.loads(response.text)
	currentTemp = data['devices'][0]['traits']['sdm.devices.traits.ThermostatTemperatureSetpoint']['coolCelsius']
	return currentTemp

def update_TherMostat(TEMP):
	'''
	params: Temperature float value

	Send this value via the POST request to update Google NEST.

	'''
	data = '{ "command" : "sdm.devices.commands.ThermostatTemperatureSetpoint.SetCool", "params" : { "coolCelsius" :'+str(TEMP) +'} }'

	response = requests.post('https://smartdevicemanagement.googleapis.com/v1/enterprises/88e358d5-7a5d-42d1-aeb8-a487f595fa89/devices/AVPHwEsaaetpl_vdFsoiTy1D_0uOrR7iQV2IiExGbsqn2F2AoFNvQ9VDFP5CmDhd7QgaTrak3eaBmmMaAZz-B_NwifYqKQ:executeCommand', headers=HEADERS, data=data)

if __name__ == '__main__':
	print("/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/-/ \n")
	temp_diff_from_atmega = Get_Temp_From_Atmega()
	if temp_diff_from_atmega == None:
		print("TIME-{0} Status: No Data Received".format(time.asctime( time.localtime(time.time()))))

	else:
		current_thermostat_temp = getCurrentTemp()
		new_temp_to_update = current_thermostat_temp - temp_diff_from_atmega 
		

		update_TherMostat(new_temp_to_update)
		
		print("TIME:{0} \n Status: Data Received \n SENSOR_READING_TEMPERATURE_DIFF: {1} \n NEST-READING: {2} \n NEW TEMPERATURE: {3} \n".format(time.asctime( time.localtime(time.time())),temp_diff_from_atmega,current_thermostat_temp,new_temp_to_update))



