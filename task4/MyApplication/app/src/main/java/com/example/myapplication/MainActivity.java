package com.example.myapplication;

import android.annotation.SuppressLint;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.widget.*;
import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;

import java.io.BufferedWriter;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.UnknownHostException;

public class MainActivity extends AppCompatActivity {
    private static final String TAG = "lzhj";
    RelativeLayout mTouchpadLayout;
    ImageView mMouseLeftImageView;
    ImageView mMouseRightImageView;
    ImageView mMouseMiddleImageView;
    FrameLayout mTouchpaneLayout;


    LinearLayout mSetupLayout;
    EditText mServerIPEditText;
    EditText mServerPortText;
    Button mConnectButton;

    Socket mSocket;
    PrintWriter mWriter;

    Handler handler;

    @SuppressLint("ClickableViewAccessibility")
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        mTouchpadLayout = (RelativeLayout) findViewById(R.id.touchpad_layout);
        mMouseLeftImageView = (ImageView) findViewById(R.id.mouse_left);
        mMouseMiddleImageView = (ImageView) findViewById(R.id.mouse_middle_btn);
        mMouseRightImageView = (ImageView) findViewById(R.id.mouse_right);
        mTouchpaneLayout = (FrameLayout) findViewById(R.id.touch_panel);

        mSetupLayout = (LinearLayout) findViewById(R.id.setup_ll);
        mServerIPEditText = (EditText) findViewById(R.id.serverip_et);
        mServerPortText = (EditText) findViewById(R.id.port_et);
        mConnectButton = (Button) findViewById(R.id.connect_bt);



        mConnectButton.setOnClickListener(new View.OnClickListener() {

            @SuppressLint("HandlerLeak")
            @Override
            //单击函数
            public void onClick(View v) {

                handler = new Handler() {
                    @Override
                    public void handleMessage(@NonNull Message msg) {
                        mTouchpadLayout.setVisibility(View.VISIBLE);
                    }
                };

                new Thread(new Runnable() {

                    @Override
                    public void run() {
                        try {
                            Log.d(TAG, "stub 1 ");//记录日志语句，不是重点

                            mSocket = new Socket(mServerIPEditText.getText().toString(), Integer.parseInt(mServerPortText.getText().toString()));

                            System.out.println(mSocket.getLocalAddress() + ":" + mSocket.getLocalPort());

                            Log.d(TAG, "stub 2 ");


                            //定义一个输出流，将信息全部加载到输出流中发送给服务器。这些是利用已经建立连接的mSocket
                            mWriter = new PrintWriter(new BufferedWriter(new OutputStreamWriter(mSocket.getOutputStream())));
                            Log.d(TAG, "stub 3 ");

                            if (mSocket.isConnected()) {
                                handler.sendEmptyMessage(0);
                            }
                            Log.d(TAG, "mSocket.isConnected() = " + mSocket.isConnected());

                        } catch (NumberFormatException e) {
                            e.printStackTrace();
                        } catch (UnknownHostException e) {
                            e.printStackTrace();
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                    }
                }).start();
            }
        });


        //点击屏幕中的“鼠标左键”动作
        mMouseLeftImageView.setOnTouchListener(new View.OnTouchListener() {

            @Override
            public boolean onTouch(View v, MotionEvent event) {

                String action = null;
                //监听动作类型
                switch (event.getAction()) {

                    //如果动作类型是点下
                    case MotionEvent.ACTION_DOWN:
                        action = "MOUSEEVENTF_LEFTDOWN";
                        break;

                    case MotionEvent.ACTION_UP:
                        action = "MOUSEEVENTF_LEFTUP";
                        break;
                    default:
                        break;
                }
                //如果action祖父传不为空
                if (action != null) {
                    //调用sendSocketMessage函数，将action这个字符串发给服务器
                    sendSocketMessage(action);
                }
                return true;
            }
        });


        //点击屏幕中的“鼠标右键”动作
        mMouseRightImageView.setOnTouchListener(new View.OnTouchListener() {

            @Override
            public boolean onTouch(View v, MotionEvent event) {
                String action = null;
                switch (event.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        action = "MOUSEEVENTF_RIGHTDOWN";
                        break;
                    case MotionEvent.ACTION_UP:
                        action = "MOUSEEVENTF_RIGHTUP";
                        break;
                    default:
                        break;
                }
                if (action != null) {
                    sendSocketMessage(action);
                }
                return true;
            }
        });


        //点击屏幕中的“鼠标滚轮”动作
        mMouseMiddleImageView.setOnTouchListener(new View.OnTouchListener() {
            float lastX;
            float lastY;

            @Override
            public boolean onTouch(View v, MotionEvent event) {
                String action = null;
                switch (event.getAction()) {
                    case MotionEvent.ACTION_MOVE:
                        lastY = event.getY();
                        if (event.getY() - lastY < 0) {
                            action = "MOUSEEVENTF_SCROLLUP";
                        } else if (event.getY() - lastY > 0) {
                            action = "MOUSEEVENTF_SCROLLDOWN";
                        }
                        break;
                    default:
                        break;
                }
                if (action != null) {
                    sendSocketMessage(action);
                }
                return true;
            }
        });

        //鼠标面板滑动动作，就是在这个区域内滑动所要产生的信息处理
        mTouchpaneLayout.setOnTouchListener(new View.OnTouchListener() {
            //对位置坐标的定义
            float lastX;
            float lastY;

            @SuppressLint("ClickableViewAccessibility")
            @Override
            public boolean onTouch(View v, MotionEvent event) {//当点击面板时
                String actionString = null;
                //检测用户动作
                switch (event.getAction()) {
                    case MotionEvent.ACTION_DOWN:
                        lastX = event.getX();
                        lastY = event.getY();
                        break;

                    case MotionEvent.ACTION_MOVE:
                        actionString = String.format("MOUSEEVENTF_MOVE,%f,%f", event.getX() - lastX, event.getY() - lastY);
                        lastX = event.getX();
                        lastY = event.getY();

                        break;
                    case MotionEvent.ACTION_UP:
                 break;

                    default:
                        break;
                }
                if (actionString != null) {
                    sendSocketMessage(actionString);
                }
                return true;
            }
        });
    }

    //与服务器断开连接的动作
    public void stopConnection() {
        if (mSocket.isConnected()) {
            try {
                //先关闭流
                mWriter.close();
                //在关闭socket
                mSocket.close();
            } catch (IOException e) {
                e.printStackTrace();
            }

        }
    }



    public void sendSocketMessage(final String message) {
        if (!mSocket.isConnected()) {
            Toast.makeText(this, "connetion lost", Toast.LENGTH_SHORT).show();
            mTouchpadLayout.setVisibility(View.GONE);

            mWriter.close();
            try {
                mSocket.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
            return;
        }

        new Thread(new Runnable() {
            @Override
            public void run() {
                mWriter.println(message);
                mWriter.flush();
            }
        }).start();
    }


    @Override
    protected void onResume() {
        //仅使用系统本身的管理动作，没有自定义更多
        // TODO Auto-generated method stub
        super.onResume();
    }

    @Override
    protected void onPause() {
        //仅使用系统本身的管理动作，没有自定义更多
        // TODO Auto-generated method stub
        super.onPause();
    }


    //点击系统back键的动作
    @Override
    public void onBackPressed() {
        if (mTouchpadLayout.getVisibility() == View.VISIBLE) {
            //调用之前定义的停止连接函数
            stopConnection();
            mTouchpadLayout.setVisibility(View.GONE);
            return;
        }
        super.onBackPressed();
    }
}