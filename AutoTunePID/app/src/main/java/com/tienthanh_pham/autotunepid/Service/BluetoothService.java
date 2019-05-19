package com.tienthanh_pham.autotunepid.Service;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.os.Handler;

import com.tienthanh_pham.autotunepid.MainActivityPresenter;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Set;
import java.util.UUID;

public class BluetoothService {

    MainActivityPresenter mPresenter;
    BluetoothAdapter mBluetoothAdapter;
    BluetoothSocket mmSocket;
    BluetoothDevice mmDevice;
    OutputStream mmOutputStream;
    InputStream mmInputStream;
    Thread workerThread;
    byte[] readBuffer;
    int readBufferPosition;
    volatile boolean stopWorker;

    public BluetoothService(MainActivityPresenter presenter){
        mPresenter = presenter;
        try {
            initialize();
        } catch (IOException ex){

        }
    }

    void initialize() throws IOException {
        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if(mBluetoothAdapter==null)
        {
            mPresenter.showMessage("Does not support bluetooth");
        } else if (!mBluetoothAdapter.isEnabled()) {
            mPresenter.showMessage("Please turn on the Bluetooth");
        } else {
            mPresenter.showMessage("its good!");
            Set<BluetoothDevice> pairedDevices = mBluetoothAdapter.getBondedDevices();

            if (pairedDevices.size() > 0) {
                // There are paired devices. Get the name and address of each paired device.
                for (BluetoothDevice device : pairedDevices) {
                    mmDevice = device;
                    String deviceName = device.getName();
                    String deviceHardwareAddress = device.getAddress(); // MAC address
                    mPresenter.showMessage(deviceName + deviceHardwareAddress);
                }
                openBT();
            }
        }
    }

    public void openBT() {
        try {
            UUID uuid = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB"); //Standard SerialPortService ID
            mmSocket = mmDevice.createRfcommSocketToServiceRecord(uuid);
            mmSocket.connect();
            mmOutputStream = mmSocket.getOutputStream();
            mmInputStream = mmSocket.getInputStream();

            beginListenForData();

            mPresenter.showMessage("Bluetooth Opened");
        } catch (IOException ex){

        }
    }

    void beginListenForData() {
        final Handler handler = new Handler();
        final byte delimiter = 10; //This is the ASCII code for a newline character

        stopWorker = false;
        readBufferPosition = 0;
        readBuffer = new byte[1024];
        workerThread = new Thread(new Runnable() {
            public void run() {
                while (!Thread.currentThread().isInterrupted() && !stopWorker) {
                    try {
                        int bytesAvailable = mmInputStream.available();
                        if (bytesAvailable > 0) {
                            byte[] packetBytes = new byte[bytesAvailable];
                            mmInputStream.read(packetBytes);
                            for (int i = 0; i < bytesAvailable; i++) {
                                byte b = packetBytes[i];
                                if (b == delimiter) {
                                    byte[] encodedBytes = new byte[readBufferPosition];
                                    System.arraycopy(readBuffer, 0, encodedBytes, 0, encodedBytes.length);
                                    final String data = new String(encodedBytes, "US-ASCII");
                                    readBufferPosition = 0;

                                    handler.post(new Runnable() {
                                        public void run() {
                                            mPresenter.updateData(data);
                                        }
                                    });
                                } else {
                                    readBuffer[readBufferPosition++] = b;
                                }
                            }
                        }
                    } catch (IOException ex) {
                        stopWorker = true;
                    }
                }
            }
        });

        workerThread.start();
    }

    public void sendData(String data) {
        String msg = data + "\n";
        try {
            mmOutputStream.write(msg.getBytes());
//            mPresenter.showMessage("Data Sent" + data);
        } catch (IOException ex){
            mPresenter.showMessage(ex.toString());
        }
    }

    public void closeBT() {
        try {
            stopWorker = true;
            mmOutputStream.close();
            mmInputStream.close();
            mmSocket.close();
            mPresenter.showMessage("Bluetooth Closed");
        } catch (IOException ex){

        }
    }
}
