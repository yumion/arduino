//BLE nano 2
#include <nRF5x_BLE_API.h>


#define RX_BUF_LEN 1  //ここを増やすとRecvが増える
#define DEVICE_NAME  "MyBlePeripheral"

BLE ble;

uint8_t rxPayload[RX_BUF_LEN] = {0,}; //受信配列

// The Nordic UART Service
static const uint8_t uart_base_uuid_rev[]   = {0x1E, 0x94, 0x8D, 0xF1, 0x48, 0x31, 0x94, 0xBA, 0x75, 0x4C, 0x3E, 0x50, 0, 0, 0x3D, 0x71};
static const uint8_t service_uuid[]        = {0x71, 0x3D, 0, 0, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};
static const uint8_t service_rx_uuid[] = {0x71, 0x3D, 0, 1, 0x50, 0x3E, 0x4C, 0x75, 0xBA, 0x94, 0x31, 0x48, 0xF1, 0x8D, 0x94, 0x1E};

//Centralからのデータのみもらう
GattCharacteristic rx_characteristic(service_rx_uuid, rxPayload, 1, RX_BUF_LEN, GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE);

GattCharacteristic *uartChars[] = {&rx_characteristic};
GattService uartService(service_uuid, uartChars, sizeof(uartChars) / sizeof(GattCharacteristic *));


//connect task
void connectionCallBack( const Gap::ConnectionCallbackParams_t *params ) {
  Serial.println("Conect");
   if(params->role == Gap::PERIPHERAL) {
    Serial.println("Peripheral ");
  }
}

//disconect tack
void disconnectionCallBack(const Gap::DisconnectionCallbackParams_t *params) {
  Serial.print("Disconnected hande : ");
  Serial.println(params->handle);
  Serial.print("Disconnected reason : ");
  Serial.println(params->reason);
  Serial.println("Restart advertising ");
  ble.startAdvertising();
}


/**
 * 書き込み要求がきたときに呼び出されるやつ
 * Readもできるよ！
 */
void RecvHandler(const GattWriteCallbackParams *Handler){
  uint8_t buf[RX_BUF_LEN];
  uint16_t bytesRead;

   if (Handler->handle == rx_characteristic.getValueAttribute().getHandle()){
     ble.readCharacteristicValue(rx_characteristic.getValueAttribute().getHandle(), buf, &bytesRead);

     memset(rxPayload, 0, RX_BUF_LEN); //初期化
     memcpy(rxPayload, buf, RX_BUF_LEN); //配列にぶっこむ

     Serial.println(buf[0]);       //バイト型をもらう
     Serial.println( (char*)buf ); //バイト型を文字に変換

     if(buf[0] == 0){//0x00を受け取ると13pinが消える
       digitalWrite(13, 0);
     }else if(buf[0] == 1){//0x00を受け取ると13pinが点灯する
       digitalWrite(13, 1);
     }

   }
}


void SetAdvertisement(){
  // A list of Advertising Data types commonly used by peripherals.
  ble.accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
  // Add complete 128bit_uuid to advertisement
  ble.accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_128BIT_SERVICE_IDS,(const uint8_t *)uart_base_uuid_rev, sizeof(uart_base_uuid_rev));
  // Add complete device name to scan response data
  ble.accumulateScanResponse(GapAdvertisingData::COMPLETE_LOCAL_NAME,(const uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME) - 1);
}

void setup() {
  Serial.begin(115200);

  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  //BLE設定
  ble.init();
  ble.onConnection(connectionCallBack);
  ble.onDisconnection(disconnectionCallBack);
  ble.onDataWritten(RecvHandler); //WriteだけどReadでもらう

  //set advertisement
  SetAdvertisement();

  // set adv_type
  ble.setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
  // add service
  ble.addService(uartService);
  // set device name
  ble.setDeviceName((const uint8_t *)DEVICE_NAME);
  // set tx power,valid values are -40, -20, -16, -12, -8, -4, 0, 4
  ble.setTxPower(4);
  // set adv_interval, 100ms in multiples of 0.625ms.
  ble.setAdvertisingInterval(160);
  // set adv_timeout, in seconds
  ble.setAdvertisingTimeout(0);
  // start advertising
  ble.startAdvertising();

}

void loop() {
  ble.waitForEvent();
}｀
