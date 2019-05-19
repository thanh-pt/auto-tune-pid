package com.tienthanh_pham.autotunepid;

import android.graphics.Color;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

import com.github.mikephil.charting.charts.LineChart;
import com.github.mikephil.charting.data.Entry;
import com.github.mikephil.charting.data.LineData;
import com.github.mikephil.charting.data.LineDataSet;

import java.util.ArrayList;

public class MainActivity extends AppCompatActivity implements MainActivityPresenter.View{

    private MainActivityPresenter mPresenter;
    private LineChart chart;
    private ArrayList<Entry> mCurrentSpeed;
    private LineDataSet mLDSCurrentSpeed;
    private ArrayList<Entry> mSetpoint;
    private LineDataSet mLDSSetPoint;

    public MainActivity() {
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        messageShow("Ứng dụng bắt đầu chạy!");

        mPresenter = new MainActivityPresenter(this);

        final ToggleButton toggleOnOff = findViewById(R.id.btnOnOff);
        toggleOnOff.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                mPresenter.sentCmd(0, isChecked ? "1" : "0");
            }
        });

        final Button setPoint = findViewById(R.id.btnSetPoint);
        setPoint.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                EditText txtSetPoint = findViewById(R.id.txtSetPoint);
                mPresenter.sentCmd(2, txtSetPoint.getText().toString());
            }
        });

        final Button setPID = findViewById(R.id.btnSetPID);
        setPID.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                EditText txtPID = findViewById(R.id.txtPID);
                mPresenter.sentCmd(3, txtPID.getText().toString());
            }
        });

        final ToggleButton toggleOnOffBluetooth = findViewById(R.id.btnOnOffBluetooth);
        toggleOnOffBluetooth.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                mPresenter.sentCmd(4, isChecked ? "1" : "0");
            }
        });

        lineChartInit();
    }


    @Override
    public void updateChart(int setPoint, int currentSpeech) {
        mSetpoint.add(new Entry(mSetpoint.size(), setPoint));
        mCurrentSpeed.add(new Entry(mCurrentSpeed.size(), currentSpeech));
        if (chart.getData() != null &&
                chart.getData().getDataSetCount() > 0) {
            mLDSCurrentSpeed = (LineDataSet) chart.getData().getDataSetByIndex(0);
            mLDSCurrentSpeed.setValues(mCurrentSpeed);
            mLDSSetPoint = (LineDataSet) chart.getData().getDataSetByIndex(1);
            mLDSSetPoint.setValues(mSetpoint);
            chart.getData().notifyDataChanged();
            chart.notifyDataSetChanged();

            chart.setVisibleXRangeMaximum(150);
            chart.moveViewToX(chart.getData().getEntryCount());
        }else {
            mLDSCurrentSpeed = new LineDataSet(mCurrentSpeed, "Current Speed");
            mLDSCurrentSpeed.setLineWidth(2f);
            mLDSCurrentSpeed.setCircleRadius(3f);
            mLDSCurrentSpeed.setFillAlpha(65);
            mLDSCurrentSpeed.setColor(Color.RED);
            mLDSCurrentSpeed.setDrawCircles(false);

            mLDSSetPoint = new LineDataSet(mCurrentSpeed, "Set point");
            mLDSSetPoint.setLineWidth(2f);
            mLDSSetPoint.setCircleRadius(3f);
            mLDSSetPoint.setFillAlpha(65);
            mLDSSetPoint.setDrawCircles(false);

            LineData data = new LineData(mLDSCurrentSpeed, mLDSSetPoint);
            chart.setData(data);
        }
    }

    @Override
    public void messageShow(String msg) {
        Toast.makeText(getApplicationContext(), msg, Toast.LENGTH_SHORT).show();
    }

    private void lineChartInit(){
        chart = findViewById(R.id.lineChart);
        chart.setScaleEnabled(false);
        chart.setDragEnabled(false);
        chart.setTouchEnabled(false);
        chart.setDrawGridBackground(false);
        chart.setPinchZoom(false);

        mCurrentSpeed = new ArrayList<>();
        mSetpoint = new ArrayList<>();
        updateChart(0, 0);
    }
}
