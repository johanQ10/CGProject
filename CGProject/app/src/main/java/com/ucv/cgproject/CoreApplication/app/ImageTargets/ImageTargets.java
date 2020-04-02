/*===============================================================================
Copyright (c) 2019 PTC Inc. All Rights Reserved.

Copyright (c) 2012-2014 Qualcomm Connected Experiences, Inc. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other 
countries.
===============================================================================*/

package com.ucv.cgproject.CoreApplication.app.ImageTargets;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.res.AssetManager;
import android.content.res.Configuration;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.graphics.ImageFormat;
import android.graphics.Point;
import android.graphics.Rect;
import android.graphics.SurfaceTexture;
import android.graphics.drawable.ColorDrawable;
import android.hardware.Camera;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.CaptureResult;
import android.hardware.camera2.TotalCaptureResult;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.media.Image;
import android.media.ImageReader;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.Log;
import android.util.Size;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.view.Window;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.github.chrisbanes.photoview.PhotoView;
import com.ucv.cgproject.BuildConfig;
import com.ucv.cgproject.CoreApplication.ui.SampleAppMenu.SampleAppMenu;
import com.ucv.cgproject.CoreApplication.ui.SampleAppMenu.SampleAppMenuInterface;
import com.ucv.cgproject.CoreApplication.ui.SampleAppMessage;
import com.ucv.cgproject.R;
import com.ucv.cgproject.SampleApplication.SampleActivityBase;
import com.ucv.cgproject.SampleApplication.SampleApplicationControl;
import com.ucv.cgproject.SampleApplication.SampleApplicationException;
import com.ucv.cgproject.SampleApplication.SampleApplicationSession;
import com.ucv.cgproject.SampleApplication.utils.LoadingDialogHandler;
import com.ucv.cgproject.SampleApplication.utils.SampleAppTimer;
import com.ucv.cgproject.SampleApplication.utils.SampleApplicationGLView;
import com.ucv.cgproject.SampleApplication.utils.Texture;
import com.vuforia.CameraDevice;
import com.vuforia.DataSet;
import com.vuforia.DeviceTracker;
import com.vuforia.ObjectTracker;
import com.vuforia.PIXEL_FORMAT;
import com.vuforia.PositionalDeviceTracker;
import com.vuforia.RuntimeImageSource;
import com.vuforia.STORAGE_TYPE;
import com.vuforia.State;
import com.vuforia.Trackable;
import com.vuforia.TrackableList;
import com.vuforia.TrackableResult;
import com.vuforia.Tracker;
import com.vuforia.TrackerManager;
import com.vuforia.Vec2I;
import com.vuforia.Vuforia;

import java.io.InputStream;
import java.lang.ref.WeakReference;
import java.nio.ByteBuffer;
import java.security.Policy;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import java.util.Vector;
import java.util.concurrent.TimeUnit;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.math.MathUtils;

/**
 * The main activity for the ImageTargets sample.
 * Image Targets allows users to create 2D targets for detection and tracking
 *
 * This class does high-level handling of the Vuforia lifecycle and any UI updates
 *
 * For ImageTarget-specific rendering, check out ImageTargetRenderer.java
 * For the low-level Vuforia lifecycle code, check out SampleApplicationSession.java
 */
public class ImageTargets extends SampleActivityBase implements SampleApplicationControl, SampleAppMenuInterface {
    private static final String LOGTAG = "ImageTargets";
    
    private SampleApplicationSession vuforiaAppSession;
    
    private DataSet mCurrentDataset;
    public final ArrayList<String> mDatasetStrings = new ArrayList<>();
    public final ArrayList<String> mRuntimeImageSources = new ArrayList<>();

    private SampleApplicationGLView mGlView;

    private ImageTargetRenderer mRenderer;
    
    private GestureDetector mGestureDetector;
    
    // The textures we will use for rendering:
    private Vector<Texture> mTextures;

    // Menu option flags
    private boolean mSwitchDatasetAsap = false;
    private boolean mFlash = false;
    private boolean mContAutofocus = true;

    private RelativeLayout mUILayout;
    
    private SampleAppMenu mSampleAppMenu;
    ArrayList<View> mSettingsAdditionalViews = new ArrayList<>();

    private SampleAppMessage mSampleAppMessage;
    private SampleAppTimer mRelocalizationTimer;
    private SampleAppTimer mStatusDelayTimer;

    private int mCurrentStatusInfo;

    final LoadingDialogHandler loadingDialogHandler = new LoadingDialogHandler(this);
    // Alert Dialog used to display SDK errors
    private AlertDialog mErrorDialog;
    private boolean mIsDroidDevice = false;

    private Dialog dialogView;


/*
    private float zoomLevel = 1f;
    private float maximumZoomLevel;
    private Rect zoomR;

    private Size previewSize;

    private String cameraId;
    private CameraDevice cameraDevice;
    private CameraCharacteristics cameraCharacteristics;
    private CameraCaptureSession captureSession;
    private CaptureRequest previewRequest;
    private CaptureRequest.Builder previewRequestBuilder;

    private int sensorOrientation;

    private OnPictureTakenListener onPictureTakenListener;

    private float aspectRatio = 0.75f;
    private static final float aspectRatioThreshold = 0.05f;

    private Handler backgroundHandler;
    private HandlerThread backgroundThread;
    private ImageReader imageReader;
*/
    private float zoomLevel = 1f;
    private float maximumZoomLevel;
    private CameraDevice mCameraDevice;
    private CameraCharacteristics cameraCharacteristics;
    private CaptureRequest.Builder previewRequestBuilder;
    private Zoom zoom;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.d(LOGTAG, "onCreate");
        super.onCreate(savedInstanceState);
        
        vuforiaAppSession = new SampleApplicationSession(this);
        
        startLoadingAnimation();
        mDatasetStrings.add("CGProject.xml");

        mRuntimeImageSources.add("reloj1.jpg");
        mRuntimeImageSources.add("reloj2.jpg");
        mRuntimeImageSources.add("reloj3.jpg");
        mRuntimeImageSources.add("paint1.jpg");
        mRuntimeImageSources.add("paint2.jpg");
        mRuntimeImageSources.add("paint3.jpg");
        mRuntimeImageSources.add("paint4.jpg");
        mRuntimeImageSources.add("paint5.jpg");
        mRuntimeImageSources.add("solar1.jpg");
        mRuntimeImageSources.add("solar2.jpg");
        mRuntimeImageSources.add("tens1.jpg");
        mRuntimeImageSources.add("tens2.jpg");
        mRuntimeImageSources.add("tens3.jpg");
        mRuntimeImageSources.add("tens4.jpg");
        mRuntimeImageSources.add("tens5.jpg");
        mRuntimeImageSources.add("tens6.jpg");
        mRuntimeImageSources.add("bear1.jpg");
        mRuntimeImageSources.add("bear2.jpg");
        mRuntimeImageSources.add("brazuca1.jpg");
        mRuntimeImageSources.add("brazuca2.jpg");
        mRuntimeImageSources.add("brazuca3.jpg");
        mRuntimeImageSources.add("brazuca4.jpg");
        mRuntimeImageSources.add("brazuca5.jpg");
        mRuntimeImageSources.add("brazuca6.jpg");
        mRuntimeImageSources.add("brazuca7.jpg");
        mRuntimeImageSources.add("brazuca8.jpg");
        mRuntimeImageSources.add("brazuca9.jpg");
        mRuntimeImageSources.add("brazuca10.jpg");
        mRuntimeImageSources.add("brazuca11.jpg");
        mRuntimeImageSources.add("brazuca12.jpg");
        mRuntimeImageSources.add("brazuca13.jpg");
        mRuntimeImageSources.add("abuelo.jpg");
        mRuntimeImageSources.add("abuela.jpg");
        
        vuforiaAppSession.initAR(this, ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
        
        mGestureDetector = new GestureDetector(getApplicationContext(), new GestureListener(this));
        
        // Load any sample specific textures:
        mTextures = new Vector<>();
        loadTextures();

        mIsDroidDevice = android.os.Build.MODEL.toLowerCase().startsWith("droid");

        // Relocalization timer and message
        mSampleAppMessage = new SampleAppMessage(this, mUILayout, mUILayout.findViewById(R.id.topbar_layout), false);
        mRelocalizationTimer = new SampleAppTimer(10000, 1000) {
            @Override
            public void onFinish() {
                if (vuforiaAppSession != null)
                    vuforiaAppSession.resetDeviceTracker();
                super.onFinish();
            }
        };

        mStatusDelayTimer = new SampleAppTimer(1000, 1000) {
            @Override
            public void onFinish() {
                if (mRenderer.isTargetCurrentlyTracked()) {
                    super.onFinish();
                    return;
                }

                if (!mRelocalizationTimer.isRunning())
                    mRelocalizationTimer.startTimer();

                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        mSampleAppMessage.show(getString(R.string.instruct_relocalize));
                    }
                });

                super.onFinish();
            }
        };
    }

    private static class GestureListener extends GestureDetector.SimpleOnGestureListener {
        // Used to set autofocus one second after a manual focus is triggered
        private final Handler autofocusHandler = new Handler();

        private WeakReference<ImageTargets> activityRef;

        private GestureListener(ImageTargets activity) {
            activityRef = new WeakReference<>(activity);
        }

        @Override
        public boolean onDown(MotionEvent e)
        {
            return true;
        }

        // Process Single Tap event to trigger autofocus
        @Override
        public boolean onSingleTapUp(MotionEvent e) {
            boolean result = CameraDevice.getInstance().setFocusMode(CameraDevice.FOCUS_MODE.FOCUS_MODE_TRIGGERAUTO);
            if (!result)
                Log.e("SingleTapUp", "Unable to trigger focus");

            // Generates a Handler to trigger continuous auto-focus
            // after 1 second
            autofocusHandler.postDelayed(new Runnable()
            {
                public void run()
                {
                    if (activityRef.get().mContAutofocus) {
                        final boolean autofocusResult = CameraDevice.getInstance().setFocusMode(
                                CameraDevice.FOCUS_MODE.FOCUS_MODE_CONTINUOUSAUTO);

                        if (!autofocusResult)
                            Log.e("SingleTapUp", "Unable to re-enable continuous auto-focus");
                    }
                }
            }, 1000L);

            return true;
        }
    }

    // Load specific textures from the APK, which we will later use for rendering.
    private void loadTextures() {
        mTextures.add(Texture.loadTextureFromApk("textClock.png", getAssets()));
        mTextures.add(Texture.loadTextureFromApk("textPaint.png", getAssets()));
        mTextures.add(Texture.loadTextureFromApk("textSolar.png", getAssets()));
        mTextures.add(Texture.loadTextureFromApk("textTens.png", getAssets()));
        mTextures.add(Texture.loadTextureFromApk("textBear.png", getAssets()));
        mTextures.add(Texture.loadTextureFromApk("textBrazuca.png", getAssets()));
        mTextures.add(Texture.loadTextureFromApk("Abuelo.png", getAssets()));
        mTextures.add(Texture.loadTextureFromApk("Abuela.png", getAssets()));
        mTextures.add(Texture.loadTextureFromApk("ImageTargets/Buildings.png", getAssets()));
    }

    @SuppressLint("SourceLockedOrientationActivity")
    @Override
    protected void onResume() {
        Log.d(LOGTAG, "onResume");
        super.onResume();

        showProgressIndicator(true);
        
        // This is needed for some Droid devices to force portrait
        if (mIsDroidDevice) {
            setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
            setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
        }

        vuforiaAppSession.onResume();
    }

    // Called whenever the device orientation or screen resolution changes
    @Override
    public void onConfigurationChanged(Configuration config) {
        Log.d(LOGTAG, "onConfigurationChanged");
        super.onConfigurationChanged(config);
        vuforiaAppSession.onConfigurationChanged();
    }

    @Override
    protected void onPause() {
        Log.d(LOGTAG, "onPause");
        super.onPause();
        
        if (mGlView != null) {
            mGlView.setVisibility(View.INVISIBLE);
            mGlView.onPause();
        }
        
        vuforiaAppSession.onPause();
    }

    @Override
    protected void onDestroy() {
        Log.d(LOGTAG, "onDestroy");
        super.onDestroy();
        
        try {
            vuforiaAppSession.stopAR();
        } catch (SampleApplicationException e) {
            Log.e(LOGTAG, e.getString());
        }
        
        // Unload texture:
        mTextures.clear();
        mTextures = null;
        
        System.gc();
    }

    private void initApplicationAR() {
        // Create OpenGL ES view:
        int depthSize = 16;
        int stencilSize = 0;
        boolean translucent = Vuforia.requiresAlpha();
        
        mGlView = new SampleApplicationGLView(getApplicationContext());
        mGlView.init(translucent, depthSize, stencilSize);

        mRenderer = new ImageTargetRenderer(this, vuforiaAppSession);
        mRenderer.setTextures(mTextures);
        mGlView.setRenderer(mRenderer);
        mGlView.setPreserveEGLContextOnPause(true);

        setRendererReference(mRenderer);
    }

    private void startLoadingAnimation() {
        Log.v("TAG","1");
        mUILayout = (RelativeLayout) View.inflate(getApplicationContext(), R.layout.camera_overlay, null);
        
        mUILayout.setVisibility(View.VISIBLE);
        mUILayout.setBackgroundColor(Color.BLACK);

        RelativeLayout topbarLayout = mUILayout.findViewById(R.id.topbar_layout);
        topbarLayout.setVisibility(View.VISIBLE);

//        TextView title = mUILayout.findViewById(R.id.topbar_title);
//        title.setText(getText(R.string.feature_image_targets));

        mSettingsAdditionalViews.add(topbarLayout);
        
        // Gets a reference to the loading dialog
        loadingDialogHandler.mLoadingDialogContainer = mUILayout.findViewById(R.id.loading_indicator);
        
        // Shows the loading indicator at start
        loadingDialogHandler.sendEmptyMessage(LoadingDialogHandler.SHOW_LOADING_DIALOG);
        
        // Adds the inflated layout to the view
        addContentView(mUILayout, new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
    }

    @Override
    public boolean doLoadTrackersData() {
        TrackerManager tManager = TrackerManager.getInstance();
        ObjectTracker objectTracker = (ObjectTracker) tManager.getTracker(ObjectTracker.getClassType());

        if (objectTracker == null) return false;
        if (mCurrentDataset == null) mCurrentDataset = objectTracker.createDataSet();
        if (mCurrentDataset == null) return false;

        /// Creating a dataset with multiple image targets created from images loaded at run time using the native
        /// Java BitmapFactory class to access raw pixel data.
        /// A second version loads from the image file via Vuforia instead.
        /// This code block creates a Vuforia::DataSet containing all the images from the mRuntimeImageSources, or a nullptr if one of the
        /// Images couldn't be loaded
        ///
        /// The steps to use the Instant Image Target api are highlighted with "Instant Image Target Step <X>" in comments

        // Instant Image Target Step 1:
        // retrieve the RuntimeImageSource from the object tracker. The same instance can be used to
        // create multiple image targets
        RuntimeImageSource runtimeImageSource = objectTracker.getRuntimeImageSource();

        int apiSelector = 0;
        for(String imageFileName : mRuntimeImageSources) {

            if(apiSelector % 2 == 0) {
                // Get the AssetManager to be able to load a file from the packaged resources
                AssetManager assets = getAssets();

                Bitmap image;
                try {
                    // Initialize BitmapImage from an InputStream created from the AssetManager
                    InputStream inputStream = assets.open(imageFileName, AssetManager.ACCESS_BUFFER);
                    image = BitmapFactory.decodeStream(inputStream);
                    inputStream.close();
                } catch (java.io.IOException e) {
                    Log.e(LOGTAG, "ERROR: failed to load image");
                    objectTracker.destroyDataSet(mCurrentDataset);
                    return false;
                }

                // Get the image meta information
                int width = image.getWidth();
                int height = image.getHeight();

                int bytesPerPixel = image.getByteCount() / (width * height);

                int bytes = image.getByteCount();

                // Create a new buffer; use allocateDirect so C++ can access the data
                ByteBuffer buffer = ByteBuffer.allocateDirect(bytes);
                image.copyPixelsToBuffer(buffer);

                // Calculate the Vuforia::PixelFormat based on the number of bytes used to represent a pixel in the image
                int format;
                switch (bytesPerPixel) {
                    case 1:
                        format = PIXEL_FORMAT.GRAYSCALE;
                        break;
                    case 3:
                        format = PIXEL_FORMAT.RGB888;
                        break;
                    case 4:
                        format = PIXEL_FORMAT.RGBA8888;
                        break;
                    default:
                        format = PIXEL_FORMAT.UNKNOWN_FORMAT;
                        return false;
                }

                String targetName = imageFileName.substring(0, imageFileName.lastIndexOf('.'));
                // Instant Image Target Step 2:
                // Configure the RuntimeImageSource with the data from the loaded image.
                if (!runtimeImageSource.setImage(buffer, format, new Vec2I(width, height), 0.247f, targetName)) {
                    Log.e(LOGTAG, "ERROR: failed to load from image");
                    objectTracker.destroyDataSet(mCurrentDataset);
                    return false;
                }
            } else {
                // Instant Image Target Step 2:
                // Configure the RuntimeImageSource with path to the file and the path type (see STORAGE_TYPE for options)
                String targetName = imageFileName.substring(0, imageFileName.lastIndexOf('.'));
                if (!runtimeImageSource.setFile(imageFileName, STORAGE_TYPE.STORAGE_APPRESOURCE, 0.247f, targetName)) {
                    Log.e(LOGTAG, "ERROR: failed to load from image");
                    objectTracker.destroyDataSet(mCurrentDataset);
                    return false;
                }
            }
            // Instant Image Target Step 3:
            // Use the RuntimeImageSource instance to create the Trackable in the specified Vuforia::DataSet.
            mCurrentDataset.createTrackable(runtimeImageSource);
            ++apiSelector;
        }

        if (!objectTracker.activateDataSet(mCurrentDataset)) return false;
        
        TrackableList trackableList = mCurrentDataset.getTrackables();
        for (Trackable trackable : trackableList) {
            String name = "Current Dataset : " + trackable.getName();
            trackable.setUserData(name);
            Log.d(LOGTAG, "UserData:Set the following user data " + trackable.getUserData());
        }
        
        return true;
    }

    @Override
    public boolean doUnloadTrackersData() {
        // Indicate if the trackers were unloaded correctly
        boolean result = true;
        
        TrackerManager tManager = TrackerManager.getInstance();
        ObjectTracker objectTracker = (ObjectTracker) tManager.getTracker(ObjectTracker.getClassType());

        if (objectTracker == null) return false;
        
        if (mCurrentDataset != null && mCurrentDataset.isActive()){
            if (objectTracker.getActiveDataSets().at(0).equals(mCurrentDataset) && !objectTracker.deactivateDataSet(mCurrentDataset))
                result = false;
            else if (!objectTracker.destroyDataSet(mCurrentDataset))
                result = false;
            
            mCurrentDataset = null;
        }
        
        return result;
    }

    @Override
    public void onVuforiaResumed() {
        if (mGlView != null) {
            mGlView.setVisibility(View.VISIBLE);
            mGlView.onResume();
        }
    }

    @Override
    public void onVuforiaStarted() {
        mRenderer.updateRenderingPrimitives();

        boolean result;

        result = CameraDevice.getInstance().setFlashTorchMode(mFlash);
        Log.v(LOGTAG, "Flash: " + result);
        result = CameraDevice.getInstance().setFocusMode(CameraDevice.FOCUS_MODE.FOCUS_MODE_NORMAL);
        Log.v(LOGTAG, "Autofocus: " + result);
        /*
        if(!CameraDevice.getInstance().setFocusMode(CameraDevice.FOCUS_MODE.FOCUS_MODE_CONTINUOUSAUTO)) {
            if (!CameraDevice.getInstance().setFocusMode(CameraDevice.FOCUS_MODE.FOCUS_MODE_TRIGGERAUTO))
                CameraDevice.getInstance().setFocusMode(CameraDevice.FOCUS_MODE.FOCUS_MODE_NORMAL);
        }
         */
        showProgressIndicator(false);
    }

    private void showProgressIndicator(boolean show) {
        if (show)
            loadingDialogHandler.sendEmptyMessage(LoadingDialogHandler.SHOW_LOADING_DIALOG);
        else
            loadingDialogHandler.sendEmptyMessage(LoadingDialogHandler.HIDE_LOADING_DIALOG);
    }

    // Called once Vuforia has been initialized or
    // an error has caused Vuforia initialization to stop
    @Override
    public void onInitARDone(SampleApplicationException exception) {
        if (exception == null) {
            initApplicationAR();
            
            mRenderer.setActive();
            
            // Now add the GL surface view. It is important
            // that the OpenGL ES surface view gets added
            // BEFORE the camera is started and video
            // background is configured.
            addContentView(mGlView, new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
            
            // Sets the UILayout to be drawn in front of the camera
            mUILayout.bringToFront();

            mUILayout.setBackgroundColor(Color.TRANSPARENT);

            mSampleAppMenu = new SampleAppMenu(this, this, getString(R.string.menu_title), mGlView, mUILayout, mSettingsAdditionalViews);
            mSampleAppMenu.attachMenu();
//            setSampleAppMenuSettings();

            vuforiaAppSession.startAR();

/*
            CameraManager manager = (CameraManager) getSystemService(Context.CAMERA_SERVICE);
            String cameraId = null;
            int facing = CameraCharacteristics.LENS_FACING_BACK;

            try {
                if (manager != null) {
                    for (String each : manager.getCameraIdList()) {
                        if (facing == manager.getCameraCharacteristics(each).get(CameraCharacteristics.LENS_FACING)) {
                            cameraId = each;
                            break;
                        }
                    }

                    CameraCharacteristics characteristics = manager.getCameraCharacteristics(cameraId);
                    cameraCharacteristics = characteristics;

                    maximumZoomLevel = characteristics.get(CameraCharacteristics.SCALER_AVAILABLE_MAX_DIGITAL_ZOOM);

                    zoom = new Zoom(characteristics);


                }
                if (cameraId == null) throw new Exception("No correct facing camera is found.");

            } catch (final Exception e) {
                e.printStackTrace();
            }
*/

            mUILayout.findViewById(R.id.icon_menu).setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    mSampleAppMenu.toggleMenu();
                }
            });

            mUILayout.findViewById(R.id.icon_flash).setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    mFlash = !mFlash;
                    CameraDevice.getInstance().setFlashTorchMode(mFlash);
                }
            });

            mUILayout.findViewById(R.id.icon_zoom_in).setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
//                    zoom.setZoom(,zoomLevel++);
                }
            });

            mUILayout.findViewById(R.id.icon_zoom_out).setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
//                    zoom.setZoom(,zoomLevel--);
                }
            });

        } else {
            Log.e(LOGTAG, exception.getString());
            showInitializationErrorMessage(exception.getString());
        }
    }

    private void showInitializationErrorMessage(String message) {
        final String errorMessage = message;
        runOnUiThread(new Runnable() {
            public void run() {
                if (mErrorDialog != null)
                    mErrorDialog.dismiss();

                // Generates an Alert Dialog to show the error message
                AlertDialog.Builder builder = new AlertDialog.Builder(ImageTargets.this);
                builder
                    .setMessage(errorMessage)
                    .setTitle(getString(R.string.INIT_ERROR))
                    .setCancelable(false)
                    .setIcon(0)
                    .setPositiveButton(getString(R.string.button_OK), new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int id) {
                                finish();
                            }
                        });
                
                mErrorDialog = builder.create();
                mErrorDialog.show();
            }
        });
    }

    // Called every frame
    @Override
    public void onVuforiaUpdate(State state) {
        if (mSwitchDatasetAsap) {
            mSwitchDatasetAsap = false;

            TrackerManager tm = TrackerManager.getInstance();
            ObjectTracker ot = (ObjectTracker) tm.getTracker(ObjectTracker.getClassType());

            if (ot == null || mCurrentDataset == null || ot.getActiveDataSets().at(0) == null) {
                Log.d(LOGTAG, "Failed to swap datasets");
                return;
            }
            
            doUnloadTrackersData();
            doLoadTrackersData();
        }
    }

    @Override
    public boolean doInitTrackers() {
        // Indicate if the trackers were initialized correctly
        boolean result = true;
         
        TrackerManager tManager = TrackerManager.getInstance();

        Tracker tracker = tManager.initTracker(ObjectTracker.getClassType());
        if (tracker == null) {
            Log.e(LOGTAG, "Tracker not initialized. Tracker already initialized or the camera is already started");
            result = false;
        } else
            Log.i(LOGTAG, "Tracker successfully initialized");

        // Initialize the Positional Device Tracker
        DeviceTracker deviceTracker = (PositionalDeviceTracker) tManager.initTracker(PositionalDeviceTracker.getClassType());

        if (deviceTracker != null)
            Log.i(LOGTAG, "Successfully initialized Device Tracker");
        else Log.e(LOGTAG, "Failed to initialize Device Tracker");

        return result;
    }

    @Override
    public boolean doStartTrackers() {
        // Indicate if the trackers were started correctly
        boolean result = true;

        TrackerManager trackerManager = TrackerManager.getInstance();

        Tracker objectTracker = trackerManager.getTracker(ObjectTracker.getClassType());

        if (objectTracker != null && objectTracker.start())
            Log.i(LOGTAG, "Successfully started Object Tracker");
        else {
            Log.e(LOGTAG, "Failed to start Object Tracker");
            result = false;
        }

        if (isDeviceTrackingActive()) {
            PositionalDeviceTracker deviceTracker = (PositionalDeviceTracker) trackerManager.getTracker(PositionalDeviceTracker.getClassType());

            if (deviceTracker != null && deviceTracker.start())
                Log.i(LOGTAG, "Successfully started Device Tracker");
            else
                Log.e(LOGTAG, "Failed to start Device Tracker");
        }
        
        return result;
    }

    @Override
    public boolean doStopTrackers() {
        // Indicate if the trackers were stopped correctly
        boolean result = true;

        TrackerManager trackerManager = TrackerManager.getInstance();

        Tracker objectTracker = trackerManager.getTracker(ObjectTracker.getClassType());
        if (objectTracker != null) {
            objectTracker.stop();
            Log.i(LOGTAG, "Successfully stopped object tracker");
        } else {
            Log.e(LOGTAG, "Failed to stop object tracker");
            result = false;
        }

        // Stop the device tracker
        if(isDeviceTrackingActive()) {

            Tracker deviceTracker = trackerManager.getTracker(PositionalDeviceTracker.getClassType());

            if (deviceTracker != null) {
                deviceTracker.stop();
                Log.i(LOGTAG, "Successfully stopped device tracker");
            } else
                Log.e(LOGTAG, "Could not stop device tracker");
        }

        return result;
    }
    
    @Override
    public boolean doDeinitTrackers() {
        TrackerManager tManager = TrackerManager.getInstance();

        // Indicate if the trackers were deinitialized correctly
        boolean result = tManager.deinitTracker(ObjectTracker.getClassType());
        tManager.deinitTracker(PositionalDeviceTracker.getClassType());
        
        return result;
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        // Process the Gestures
        return ((mSampleAppMenu != null && mSampleAppMenu.processEvent(event)) || mGestureDetector.onTouchEvent(event));
    }

    boolean isDeviceTrackingActive() {
        return false;
    }

    // Menu options
    private final static int CMD_HOME = 0;
    private final static int CMD_MAP = 1;
    private final static int CMD_LOCATIONS = 2;
    private final static int CMD_IMAGES = 3;

    // In this function you can define the desired behavior for each menu option
    // Each case corresponds to a menu option
    @Override
    public boolean menuProcess(int command) {
        mSampleAppMenu.toggleMenu();
        switch (command) {
            case CMD_HOME:
                mSampleAppMenu.toggleMenu();
                break;
            case CMD_MAP:
                goToMaps(getString(R.string.location),
                        getString(R.string.location_lon),
                        getString(R.string.location_lat));
                break;
            case CMD_LOCATIONS:
                dialogLocations();
                break;
            case CMD_IMAGES:
                dialogImages();
                break;
        }
        return true;
    }

    public void checkForRelocalization(final int statusInfo) {
        if (mCurrentStatusInfo == statusInfo)
            return;

        mCurrentStatusInfo = statusInfo;

        if (mCurrentStatusInfo == TrackableResult.STATUS_INFO.RELOCALIZING) {
            // If the status is RELOCALIZING, start the timer
            if (!mStatusDelayTimer.isRunning())
                mStatusDelayTimer.startTimer();
        } else {
            // If the status is not RELOCALIZING, stop the timers and hide the message
            if (mStatusDelayTimer.isRunning())
                mStatusDelayTimer.stopTimer();

            if (mRelocalizationTimer.isRunning())
                mRelocalizationTimer.stopTimer();

            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    if (mSampleAppMessage != null)
                        mSampleAppMessage.hide();
                }
            });
        }
    }

    private void showToast(String text) {
        Toast.makeText(getApplicationContext(), text, Toast.LENGTH_SHORT).show();
    }

    private void  dialogLocations() {
        dialogView = new Dialog(this);
        dialogView.requestWindowFeature(Window.FEATURE_NO_TITLE);
        dialogView.setContentView(R.layout.dialog_locations);
        if (dialogView.getWindow() != null)
            dialogView.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));

        TextView textLocation0 = dialogView.findViewById(R.id.text_location_0);
        TextView textLocation1 = dialogView.findViewById(R.id.text_location_1);
        TextView textLocation2 = dialogView.findViewById(R.id.text_location_2);
        TextView textLocation3 = dialogView.findViewById(R.id.text_location_3);
        TextView textLocation4 = dialogView.findViewById(R.id.text_location_4);
        TextView textLocation5 = dialogView.findViewById(R.id.text_location_5);
        TextView textLocation6 = dialogView.findViewById(R.id.text_location_6);
        TextView textLocation7 = dialogView.findViewById(R.id.text_location_7);

        textLocation0.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                goToMaps(getString(R.string.location_0),
                        getString(R.string.location_0_lon),
                        getString(R.string.location_0_lat));
            }
        });
        textLocation1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                goToMaps(getString(R.string.location_1),
                        getString(R.string.location_1_lon),
                        getString(R.string.location_1_lat));
            }
        });
        textLocation2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                goToMaps(getString(R.string.location_2),
                        getString(R.string.location_2_lon),
                        getString(R.string.location_2_lat));
            }
        });
        textLocation3.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                goToMaps(getString(R.string.location_3),
                        getString(R.string.location_3_lon),
                        getString(R.string.location_3_lat));
            }
        });
        textLocation4.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                goToMaps(getString(R.string.location_4),
                        getString(R.string.location_4_lon),
                        getString(R.string.location_4_lat));
            }
        });
        textLocation5.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                goToMaps(getString(R.string.location_5),
                        getString(R.string.location_5_lon),
                        getString(R.string.location_5_lat));
            }
        });
        textLocation6.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                goToMaps(getString(R.string.location_6),
                        getString(R.string.location_6_lon),
                        getString(R.string.location_6_lat));
            }
        });
        textLocation7.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                goToMaps(getString(R.string.location_7),
                        getString(R.string.location_7_lon),
                        getString(R.string.location_7_lat));
            }
        });


        dialogView.show();
    }

    private void  dialogImages() {
        dialogView = new Dialog(this, android.R.style.Theme_Black_NoTitleBar_Fullscreen);
        dialogView.requestWindowFeature(Window.FEATURE_NO_TITLE);
        dialogView.setContentView(R.layout.dialog_images);
        if (dialogView.getWindow() != null)
            dialogView.getWindow().setBackgroundDrawable(new ColorDrawable(Color.TRANSPARENT));

        final LinearLayout linearContent = dialogView.findViewById(R.id.linear_content);
        LinearLayout linearLocation0 = dialogView.findViewById(R.id.linear_location0);
        LinearLayout linearLocation1 = dialogView.findViewById(R.id.linear_location1);
        LinearLayout linearLocation2 = dialogView.findViewById(R.id.linear_location2);
        LinearLayout linearLocation3 = dialogView.findViewById(R.id.linear_location3);
        LinearLayout linearLocation4 = dialogView.findViewById(R.id.linear_location4);
        LinearLayout linearLocation5 = dialogView.findViewById(R.id.linear_location5);
        LinearLayout linearLocation6 = dialogView.findViewById(R.id.linear_location6);
        LinearLayout linearLocation7 = dialogView.findViewById(R.id.linear_location7);
        final RelativeLayout relativeDetail = dialogView.findViewById(R.id.relative_detail);
        ImageView imageArrow = dialogView.findViewById(R.id.image_arrow);
        ImageView imageClose = dialogView.findViewById(R.id.image_close);
        final PhotoView photoView = dialogView.findViewById(R.id.photo_view);

        linearContent.setVisibility(View.VISIBLE);

        linearLocation0.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                linearContent.setVisibility(View.GONE);
                relativeDetail.setVisibility(View.VISIBLE);
                photoView.setImageResource(R.drawable.auditorio);
            }
        });
        linearLocation1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                linearContent.setVisibility(View.GONE);
                relativeDetail.setVisibility(View.VISIBLE);
                photoView.setImageResource(R.drawable.banco);
            }
        });
        linearLocation2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                linearContent.setVisibility(View.GONE);
                relativeDetail.setVisibility(View.VISIBLE);
                photoView.setImageResource(R.drawable.biblioteca);
            }
        });
        linearLocation3.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                linearContent.setVisibility(View.GONE);
                relativeDetail.setVisibility(View.VISIBLE);
                photoView.setImageResource(R.drawable.decanato);
            }
        });
        linearLocation4.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                linearContent.setVisibility(View.GONE);
                relativeDetail.setVisibility(View.VISIBLE);
                photoView.setImageResource(R.drawable.escuela);
            }
        });
        linearLocation5.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                linearContent.setVisibility(View.GONE);
                relativeDetail.setVisibility(View.VISIBLE);
                photoView.setImageResource(R.drawable.plaza);
            }
        });
        linearLocation6.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                linearContent.setVisibility(View.GONE);
                relativeDetail.setVisibility(View.VISIBLE);
                photoView.setImageResource(R.drawable.poliedrito);
            }
        });
        linearLocation7.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                linearContent.setVisibility(View.GONE);
                relativeDetail.setVisibility(View.VISIBLE);
                photoView.setImageResource(R.drawable.sala);
            }
        });


        imageClose.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                relativeDetail.setVisibility(View.GONE);
                linearContent.setVisibility(View.VISIBLE);
            }
        });

        imageArrow.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                dialogView.dismiss();
            }
        });

        dialogView.show();
    }

    private void goToMaps(String name, String lon, String lat) {
        Intent mapIntent = new Intent().setClass(this, MapsActivity.class);

        mapIntent.putExtra("name", name);
        mapIntent.putExtra("lon", lon);
        mapIntent.putExtra("lat", lat);

        startActivity(mapIntent);
    }

}
