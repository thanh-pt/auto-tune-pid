package com.tienthanh_pham.autotunepid;

import android.util.Log;

import com.tienthanh_pham.autotunepid.Service.BluetoothService;


public class MainActivityPresenter {

    private View mView;
    private BluetoothService mBluetooth;


    MainActivityPresenter(View view){
        mView = view;
        mBluetooth = new BluetoothService(this);
    }

    public void sentCmd(int cmdId, String para){
        switch (cmdId){
            case 0:
                requestSentData(para);
                break;
            case 2:
                requestSentData("2 " +  para);
                break;
            case 3:
                requestSentData("3 " + para);
                break;
            case 4:
                if (para.compareTo("1") == 0 ){
                    mBluetooth.openBT();
                } else {
                    mBluetooth.closeBT();
                }
                break;
        }
    }

    public void requestSentData(String data){
        mBluetooth.sendData(data);
    }

    public void updateChart(int setPoint, int currentSpeed){
        mView.updateChart(setPoint, currentSpeed);
    }
    public void showMessage(String msg){
        mView.messageShow(msg);
    }

    public void updateData(String data){
        int setPoint = 0;
        int currentSpeed = 0;
        try {
            setPoint = Integer.parseInt(data.split(" ")[0]);
            currentSpeed = Integer.parseInt(data.split(" ")[1]);
        } catch (Exception ex){
        }
        updateChart(setPoint, currentSpeed);
    }

    public interface View {
        void updateChart(int setPoint, int currentSpeech);
        void messageShow(String msg);
    }
}
