package com.tienthanh_pham.autotunepid;

import com.tienthanh_pham.autotunepid.Service.BluetoothService;


public class MainActivityPresenter {

    private View mView;
    private BluetoothService mBluetooth;


    MainActivityPresenter(View view){
        mView = view;
        mBluetooth = new BluetoothService(this);
    }

    public void updateText(String text){
        mView.updateText(text);
    }

    public void requestSentData(String data){
        mBluetooth.sendData(data);
    }

    public interface View {
        void updateText(String text);
    }
}
