package com.weiktonplaceint.game.gui;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.graphics.Color;
import android.graphics.PorterDuff;
import android.view.View;
import android.widget.*;

import androidx.constraintlayout.widget.ConstraintLayout;

import com.weiktonplaceint.game.R;
import com.weiktonplaceint.game.gui.util.Utils;
import java.util.Formatter;

public class Speedometer {
    public Activity activity;
    public TextView mCarHP;
    public ImageView mEngine;
    public TextView mFuel;
    public ConstraintLayout mInputLayout;
    public ImageView mLight;
    public ImageView mLock;
    public ImageView mBG;
    public TextView mMileage;
    public TextView mSpeed;

    public Speedometer(Activity activity){
        ConstraintLayout relativeLayout = activity.findViewById(R.id.speedometer);
        mInputLayout = relativeLayout;
        mBG = activity.findViewById(R.id.imageView5);
        mSpeed = activity.findViewById(R.id.speed_text);
        mFuel = activity.findViewById(R.id.speed_fuel_text);
        mCarHP = activity.findViewById(R.id.speed_car_hp_text);
        mMileage = activity.findViewById(R.id.textView2);
        mEngine = activity.findViewById(R.id.speed_engine_ico);
        mLock = activity.findViewById(R.id.speed_lock_ico);
        Utils.HideLayout(relativeLayout, false);
    }

    public void UpdateSpeedInfo(int speed, int fuel, int hp, int mileage, int engine, int light, int belt, int lock){
        hp= (int) hp/10;
        mFuel.setText(new Formatter().format("%d", Integer.valueOf(fuel)).toString());
        mMileage.setText(new Formatter().format("%06d", Integer.valueOf(mileage)).toString());
        mCarHP.setText(new Formatter().format("%d%s", Integer.valueOf(hp), "%").toString());
        mSpeed.setText(String.valueOf(speed));
        if(speed == 0) {
            mSpeed.setAlpha((float) 0.4);
            mSpeed.setText("000");
            mSpeed.setTextColor(Color.parseColor("#1a1a1a")); }
        if(speed != 0) {
            mSpeed.setAlpha((float) 1.0);
            mSpeed.setText(String.valueOf(speed));
            mSpeed.setTextColor(Color.parseColor("#FFFFFF")); }

        if(engine == 1)
            mEngine.setColorFilter(Color.parseColor("#00FF00"), PorterDuff.Mode.SRC_IN);
        else
            mEngine.setColorFilter(Color.parseColor("#FF0000"), PorterDuff.Mode.SRC_IN);

        if(lock == 1)
            mLock.setColorFilter(Color.parseColor("#00FF00"), PorterDuff.Mode.SRC_IN);
        else
            mLock.setColorFilter(Color.parseColor("#FF0000"), PorterDuff.Mode.SRC_IN);
    }

    public void ShowSpeed() {
        Utils.ShowLayout(mInputLayout, false);
    }

    public void HideSpeed() {
        Utils.HideLayout(mInputLayout, false);
    }
}