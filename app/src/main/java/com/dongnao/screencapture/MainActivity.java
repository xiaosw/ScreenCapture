package com.dongnao.screencapture;

import android.Manifest;
import android.app.Activity;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.view.View;


public class MainActivity extends Activity implements RtmpManager.Callback {


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        RtmpManager.getInstance().setCallback(this);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            requestPermissions(new String[]{Manifest.permission.RECORD_AUDIO}, 10);
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        RtmpManager.getInstance().onActivityResult(requestCode, resultCode, data);
    }

    public void stoplive(View view) {
        RtmpManager.getInstance().stopScreenCapture();
    }

    public void startlive(View view) {
        RtmpManager.getInstance().startScreenCapture(this, "rtmp://send3.douyu.com/live/3251491rcWeNgVtN?wsSecret=5a490a7d166273eb3c29eaf6de7bc38f&wsTime=59be2dbc&wsSeek=off");
    }

    //手游直播、监控操作(安全、分析行为)
    @Override
    public void onStatus(int status) {
        if (status == RtmpManager.STATUS_START) {

        } else {

        }
    }
}
