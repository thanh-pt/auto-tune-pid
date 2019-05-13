package com.tienthanh_pham.autotunepid;

import com.tienthanh_pham.autotunepid.Service.BluetoothService;


public class MainActivityPresenter {

    private View mView;
    BluetoothService mBluetooth;


    MainActivityPresenter(View view){
        mView = view;
        mBluetooth = new BluetoothService(this);
    }

    public void updateText(String text){
        mView.updateText(text);
    }

    public interface View {
        void updateText(String text);
    }
}
