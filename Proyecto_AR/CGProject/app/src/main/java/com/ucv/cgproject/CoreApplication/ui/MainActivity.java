package com.ucv.cgproject.CoreApplication.ui;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;

import com.ucv.cgproject.R;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        String mClassToLaunchPackage = getPackageName();
        String mClassToLaunch = mClassToLaunchPackage + ".CoreApplication.app.ImageTargets.ImageTargets";

        Intent i = new Intent();
        i.setClassName(getPackageName(), mClassToLaunch);
        startActivity(i);
    }
}
