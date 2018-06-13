package com.example.gschn.hw18_android;

import android.Manifest;
import android.app.Activity;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.TextureView;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ScrollView;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.TextView;

import com.hoho.android.usbserial.driver.CdcAcmSerialDriver;
import com.hoho.android.usbserial.driver.ProbeTable;
import com.hoho.android.usbserial.driver.UsbSerialDriver;
import com.hoho.android.usbserial.driver.UsbSerialPort;
import com.hoho.android.usbserial.driver.UsbSerialProber;
import com.hoho.android.usbserial.util.SerialInputOutputManager;

import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import static android.graphics.Color.blue;
import static android.graphics.Color.green;
import static android.graphics.Color.red;
import static android.graphics.Color.rgb;
import static java.lang.Math.abs;

public class MainActivity extends Activity implements TextureView.SurfaceTextureListener {

    private Camera mCamera;
    private TextureView mTextureView;
    private SurfaceView mSurfaceView;
    private SurfaceHolder mSurfaceHolder;
    private Bitmap bmp = Bitmap.createBitmap(640, 480, Bitmap.Config.ARGB_8888); //640 480
    private Canvas canvas = new Canvas(bmp);
    private Paint paint1 = new Paint();
    private TextView mTextView;
    private TextView myProgressView;

    SeekBar myControl;
    TextView myTextView;

    static long prevtime = 0; // for FPS calculation
    static int progressChanged = 30;
    static int rowObserved = 100;
    int pos1 = 0; int pos2 = 0; int pos3 = 0; int pos4 = 0;



    //SeekBar myControl2;
    //TextView myTextView1;
    //Button button;
    TextView myTextView2;
    ScrollView myScrollView;
    TextView myTextView3;

    private UsbManager manager;
    private UsbSerialPort sPort;
    private final ExecutorService mExecutor = Executors.newSingleThreadExecutor();
    private SerialInputOutputManager mSerialIoManager;

    private final SerialInputOutputManager.Listener mListener =
            new SerialInputOutputManager.Listener() {
                @Override
                public void onRunError(Exception e) {

                }

                @Override
                public void onNewData(final byte[] data) {
                    MainActivity.this.runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            MainActivity.this.updateReceivedData(data);
                        }
                    });
                }
            };

    @Override
    protected void onPause(){
        super.onPause();
        stopIoManager();
        if(sPort != null){
            try{
                sPort.close();
            } catch (IOException e){ }
            sPort = null;
        }
        finish();
    }

    @Override
    protected void onResume() {
        super.onResume();

        ProbeTable customTable = new ProbeTable();
        customTable.addProduct(0x04D8,0x000A, CdcAcmSerialDriver.class);
        UsbSerialProber prober = new UsbSerialProber(customTable);

        final List<UsbSerialDriver> availableDrivers = prober.findAllDrivers(manager);

        if(availableDrivers.isEmpty()) {
            //check
            return;
        }

        UsbSerialDriver driver = availableDrivers.get(0);
        sPort = driver.getPorts().get(0);

        if (sPort == null){
            //check
        }else{
            final UsbManager usbManager = (UsbManager) getSystemService(Context.USB_SERVICE);
            UsbDeviceConnection connection = usbManager.openDevice(driver.getDevice());
            if (connection == null){
                //check
                PendingIntent pi = PendingIntent.getBroadcast(this, 0, new Intent("com.android.example.USB_PERMISSION"), 0);
                usbManager.requestPermission(driver.getDevice(), pi);
                return;
            }

            try {
                sPort.open(connection);
                sPort.setParameters(9600, 8, UsbSerialPort.STOPBITS_1, UsbSerialPort.PARITY_NONE);

            }catch (IOException e) {
                //check
                try{
                    sPort.close();
                } catch (IOException e1) { }
                sPort = null;
                return;
            }
        }
        onDeviceStateChange();
    }

    private void stopIoManager(){
        if(mSerialIoManager != null) {
            mSerialIoManager.stop();
            mSerialIoManager = null;
        }
    }

    private void startIoManager() {
        if(sPort != null){
            mSerialIoManager = new SerialInputOutputManager(sPort, mListener);
            mExecutor.submit(mSerialIoManager);
        }
    }

    private void onDeviceStateChange(){
        stopIoManager();
        startIoManager();
    }

    private void updateReceivedData(byte[] data) {
        //do something with received data

        //for displaying:
        String rxString = null;
        try {
            rxString = new String(data, "UTF-8"); // put the data you got into a string
            myTextView3.append(rxString);
            myScrollView.fullScroll(View.FOCUS_DOWN);
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON); // keeps the screen from turning off

        myControl = (SeekBar) findViewById(R.id.seek1);

        mTextView = (TextView) findViewById(R.id.cameraStatus);

        myProgressView = (TextView) findViewById(R.id.progressview);

        //myControl2 = (SeekBar) findViewById(R.id.seek2);

//        myTextView1 = (TextView) findViewById(R.id.textView01);
//        myTextView1.setText("The value is: 20");

        myTextView2 = (TextView) findViewById(R.id.textView02);
        myScrollView = (ScrollView) findViewById(R.id.ScrollView01);
        myTextView3 = (TextView) findViewById(R.id.textView03);
        //button = (Button) findViewById(R.id.button1);

        setMyControlListener();

        // see if the app has permission to use the camera
        ActivityCompat.requestPermissions(MainActivity.this, new String[]{Manifest.permission.CAMERA}, 1);
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA) == PackageManager.PERMISSION_GRANTED) {
            mSurfaceView = (SurfaceView) findViewById(R.id.surfaceview);
            mSurfaceHolder = mSurfaceView.getHolder();

            mTextureView = (TextureView) findViewById(R.id.textureview);
            mTextureView.setSurfaceTextureListener(this);

            // set the paintbrush for writing text on the image
            paint1.setColor(0xffff0000); // red
            paint1.setTextSize(24);

            mTextView.setText("started camera");
        } else {
            mTextView.setText("no camera permissions");
        }

        //button.setOnClickListener(new View.OnClickListener() {
          //  @Override
            //public void onClick(View v) {

              //  String sendString = String.valueOf(myControl2.getProgress()) + '\n';
                //try {
                //    sPort.write(sendString.getBytes(), 10); // 10 is the timeout
                //} catch (IOException e) { }
                //myTextView2.setText("value on click is "+myControl2.getProgress());
            //}
        //});

        //setmyControl2Listener();

        manager = (UsbManager) getSystemService(Context.USB_SERVICE);
    }

    public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
        mCamera = Camera.open();
        Camera.Parameters parameters = mCamera.getParameters();
        parameters.setPreviewSize(640, 480);
        parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_INFINITY); // no autofocusing
        parameters.setAutoExposureLock(false); // keep the white balance constant, might want to turn off
        mCamera.setParameters(parameters);
        mCamera.setDisplayOrientation(90); // rotate to portrait mode

        try {
            mCamera.setPreviewTexture(surface);
            mCamera.startPreview();
        } catch (IOException ioe) {
            // Something bad happened
        }
    }

    public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {
        // Ignored, Camera does all the work for us
    }

    public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
        mCamera.stopPreview();
        mCamera.release();
        return true;
    }

    // the important function
    public void onSurfaceTextureUpdated(SurfaceTexture surface) {
        //int[] rowXsum1 = {0,0,0,0};
        //int[] count1 = {0,0,0,0};
        int rowXsum1 = 0;
        int count1 = 0;
        //int rowXsum2 = 0;
        //int count2 = 0;
        // every time there is a new Camera preview frame
        mTextureView.getBitmap(bmp);

        final Canvas c = mSurfaceHolder.lockCanvas();
        if (c != null) {
            int thresh = progressChanged; // comparison value
            int[] pixels = new int[bmp.getWidth()]; // pixels[] is the RGBA data

            //int jcount = 0;
            //for (int j = 0; j < rowObserved; j+=100) {

                //int startY = rowObserved; // which row in the bitmap to analyze to read
                for (int startY = 0; startY < bmp.getHeight(); startY += 100) {
                    bmp.getPixels(pixels, 0, bmp.getWidth(), 0, startY, bmp.getWidth(), 1);

                    // in the row, see where there is not black
                    for (int i = 0; i < bmp.getWidth(); i++) {
                        //if ((green(pixels[i]) - red(pixels[i]) > thresh) || (red(pixels[i]) > thresh) || (blue(pixels[i]) > thresh)){
                        if ((green(pixels[i]) - red(pixels[i]) > thresh) && (green(pixels[i]) - blue(pixels[i]) > thresh)) {
                            pixels[i] = rgb(0, 255, 0); // over write the pixel with pure green
                            rowXsum1 = rowXsum1 + i;
                            count1 = count1 + 1;
                            //rowXsum1[jcount] = rowXsum1[jcount] + i;
                            //count1[jcount] = count1[jcount] + 1;
                        }
                    }

                    //if (count1[jcount] == 0) {
                    //    count1[jcount] = 1;
                    //}

                    if (count1 == 0) {
                        count1 = 1;
                    }

                    if(startY == 100)
                    {
                        pos1 = (int) rowXsum1/ count1;
                    }
                    if(startY == 200)
                    {
                        pos2 = (int) rowXsum1/ count1;
                    }
                    if(startY == 300)
                    {
                        pos3 = (int) rowXsum1/ count1;
                    }
                    if(startY == 400)
                    {
                        pos4 = (int) rowXsum1/ count1;
                    }

                    // update the row
                    bmp.setPixels(pixels, 0, bmp.getWidth(), 0, startY, bmp.getWidth(), 1);

                    //jcount++;
                }

//                // draw a circle at some position
//                pos = (int) rowXsum / count;
//                canvas.drawCircle(pos, rowObserved, 5, paint1); // x position, y position, diameter, color

//                // write the pos as text
//                canvas.drawText("pos = " + pos[j], 10, rowObserved[j], paint1);
//                c.drawBitmap(bmp, 0, 0, null);
//                mSurfaceHolder.unlockCanvasAndPost(c);
            //}

        }
        // draw a circle at some position
        //pos1 = (int) rowXsum1[1] / count1[1];
        //pos2 = (int) rowXsum1[2] / count1[2];
        //pos3 = (int) rowXsum1[3] / count1[3];
        //pos4 = (int) rowXsum1[4] / count1[4];
        //pos1 = (int) rowXsum1/ count1;
        canvas.drawCircle(pos1, 100, 5, paint1); // x position, y position, diameter, color
        //canvas.drawCircle(pos2, 200, 5, paint1);
        //canvas.drawCircle(pos3, 300, 5, paint1);
        //canvas.drawCircle(pos4, 400, 5, paint1);

        // write the pos as text
        canvas.drawText("pos = " + pos2, 10, 200, paint1);
        c.drawBitmap(bmp, 0, 0, null);
        mSurfaceHolder.unlockCanvasAndPost(c);

        // calculate the FPS to see how fast the code is running
        long nowtime = System.currentTimeMillis();
        long diff = nowtime - prevtime;
        mTextView.setText("FPS " + 1000 / diff);
        prevtime = nowtime;

        int midpoint = (int) bmp.getWidth() / 2;
        int maxspeed = 2000; //max actual is 2399
        //int distance = (int) abs(pos - midpoint);

        int powerRatingLeft;
        int powerRatingRight;

        //if turn right
        if(pos2 > pos3)
        {
            powerRatingLeft = 40;// (int) * distance / midpoint;
            powerRatingRight = 0;


        }
        //if turn left
        else if(pos2 < pos3)
        {
            powerRatingLeft = 0;
            powerRatingRight = 40 ;

        }
        else
        {
            powerRatingLeft = 20;
            powerRatingRight = 20 ;
        }

        String sendString = String.valueOf((powerRatingLeft) +" "+ (powerRatingRight) + '\n');
        try {
            sPort.write(sendString.getBytes(), 10); // 10 is the timeout
        } catch (IOException e) { }
        myTextView2.setText("pos2 "+pos2+" pos3 "+pos3+" prL "+powerRatingLeft+" prR "+powerRatingRight);
    }

    private void setMyControlListener() {
        myControl.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {

            //int progressChanged = 0;

            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                progressChanged = progress + 30;
                myProgressView.setText("The value is: "+(progress+80));
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }


        });
    }

//    private void setmyControl2Listener() {
//        myControl2.setOnSeekBarChangeListener(new OnSeekBarChangeListener() {
//
//            int progressChanged = 0;
//
//            @Override
//            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
//                progressChanged = progress;
//                myTextView1.setText("The value is: "+progress);
//            }
//
//            @Override
//            public void onStartTrackingTouch(SeekBar seekBar) {
//            }
//
//            @Override
//            public void onStopTrackingTouch(SeekBar seekBar) {
//
//            }
//
//
//        });
//    }


}
