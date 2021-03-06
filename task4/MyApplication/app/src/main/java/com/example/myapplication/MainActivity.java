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
            //????????????
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
                            Log.d(TAG, "stub 1 ");//?????????????????????????????????

                            mSocket = new Socket(mServerIPEditText.getText().toString(), Integer.parseInt(mServerPortText.getText().toString()));

                            System.out.println(mSocket.getLocalAddress() + ":" + mSocket.getLocalPort());

                            Log.d(TAG, "stub 2 ");


                            //?????????????????????????????????????????????????????????????????????????????????????????????????????????????????????mSocket
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


        //??????????????????????????????????????????
        mMouseLeftImageView.setOnTouchListener(new View.OnTouchListener() {

            @Override
            public boolean onTouch(View v, MotionEvent event) {

                String action = null;
                //??????????????????
                switch (event.getAction()) {

                    //???????????????????????????
                    case MotionEvent.ACTION_DOWN:
                        action = "MOUSEEVENTF_LEFTDOWN";
                        break;

                    case MotionEvent.ACTION_UP:
                        action = "MOUSEEVENTF_LEFTUP";
                        break;
                    default:
                        break;
                }
                //??????action??????????????????
                if (action != null) {
                    //??????sendSocketMessage????????????action??????????????????????????????
                    sendSocketMessage(action);
                }
                return true;
            }
        });


        //??????????????????????????????????????????
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


        //??????????????????????????????????????????
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

        //????????????????????????????????????????????????????????????????????????????????????
        mTouchpaneLayout.setOnTouchListener(new View.OnTouchListener() {
            //????????????????????????
            float lastX;
            float lastY;

            @SuppressLint("ClickableViewAccessibility")
            @Override
            public boolean onTouch(View v, MotionEvent event) {//??????????????????
                String actionString = null;
                //??????????????????
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

    //?????????????????????????????????
    public void stopConnection() {
        if (mSocket.isConnected()) {
            try {
                //????????????
                mWriter.close();
                //?????????socket
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
        //????????????????????????????????????????????????????????????
        // TODO Auto-generated method stub
        super.onResume();
    }

    @Override
    protected void onPause() {
        //????????????????????????????????????????????????????????????
        // TODO Auto-generated method stub
        super.onPause();
    }


    //????????????back????????????
    @Override
    public void onBackPressed() {
        if (mTouchpadLayout.getVisibility() == View.VISIBLE) {
            //???????????????????????????????????????
            stopConnection();
            mTouchpadLayout.setVisibility(View.GONE);
            return;
        }
        super.onBackPressed();
    }
}