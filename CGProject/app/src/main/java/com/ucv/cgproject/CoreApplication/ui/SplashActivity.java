package com.ucv.cgproject.CoreApplication.ui;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;

import com.ucv.cgproject.R;

public class SplashActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_splash);

        long SPLASH_MILLIS = 1500;

        final Handler handler = new Handler();
        handler.postDelayed(new Runnable()
        {

            @Override
            public void run()
            {
                String mClassToLaunchPackage = getPackageName();
                String mClassToLaunch = mClassToLaunchPackage + ".CoreApplication.app.ImageTargets.ImageTargets";

                Intent i = new Intent();
                i.setClassName(getPackageName(), mClassToLaunch);
                startActivity(i);
                finish();
            }

        }, SPLASH_MILLIS);
    }
}
