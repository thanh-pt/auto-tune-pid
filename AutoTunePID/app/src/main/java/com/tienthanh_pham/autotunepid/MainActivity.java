package com.tienthanh_pham.autotunepid;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity implements MainActivityPresenter.View{

    MainActivityPresenter mPresenter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mPresenter = new MainActivityPresenter(this);

        Button btnSent = findViewById(R.id.btn_sent);

        btnSent.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                TextView inputText = findViewById(R.id.input_text);
                mPresenter.requestSentData(inputText.getText().toString());
            }
        });
    }

    @Override
    public void updateText(String text) {
        TextView targetText = findViewById(R.id.target_text);
        targetText.setText(text);
    }
}
