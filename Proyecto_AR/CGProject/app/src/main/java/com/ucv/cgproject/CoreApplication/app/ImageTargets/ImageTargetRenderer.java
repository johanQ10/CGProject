/*===============================================================================
Copyright (c) 2019 PTC Inc. All Rights Reserved.

Copyright (c) 2012-2014 Qualcomm Connected Experiences, Inc. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other 
countries.
===============================================================================*/

package com.ucv.cgproject.CoreApplication.app.ImageTargets;

import android.opengl.GLES20;
import android.opengl.Matrix;
import android.util.Log;

import com.ucv.cgproject.SampleApplication.SampleAppRenderer;
import com.ucv.cgproject.SampleApplication.SampleAppRendererControl;
import com.ucv.cgproject.SampleApplication.SampleApplicationSession;
import com.ucv.cgproject.SampleApplication.SampleRendererBase;
import com.ucv.cgproject.SampleApplication.utils.CubeShaders;
import com.ucv.cgproject.SampleApplication.utils.LoadingDialogHandler;
import com.ucv.cgproject.SampleApplication.utils.MeshObject;
import com.ucv.cgproject.SampleApplication.utils.SampleApplication3DModel;
import com.ucv.cgproject.SampleApplication.utils.SampleMath;
import com.ucv.cgproject.SampleApplication.utils.SampleUtils;
import com.ucv.cgproject.SampleApplication.utils.TextClock;
import com.ucv.cgproject.SampleApplication.utils.Texture;
import com.vuforia.DeviceTrackableResult;
import com.vuforia.ImageTargetResult;
import com.vuforia.Matrix44F;
import com.vuforia.State;
import com.vuforia.Tool;
import com.vuforia.Trackable;
import com.vuforia.TrackableResult;
import com.vuforia.TrackableResultList;
import com.vuforia.Vuforia;

import java.io.IOException;
import java.lang.ref.WeakReference;
import java.util.Vector;

import static android.opengl.GLES20.GL_ONE_MINUS_SRC_ALPHA;
import static android.opengl.GLES20.GL_SRC_ALPHA;

/**
 * The renderer class for the Image Targets sample.
 *
 * In the renderFrame() function you can render augmentations to display over the Target
 */
public class ImageTargetRenderer extends SampleRendererBase implements SampleAppRendererControl {
    private static final String LOGTAG = "ImageTargetRenderer";
    
    private final WeakReference<ImageTargets> mActivityRef;
    
    private int shaderProgramID;
    private int vertexHandle;
    private int textureCoordHandle;
    private int mvpMatrixHandle;
    private int texSampler2DHandle;

    // Object to be rendered
    private TextClock textClock;

    private boolean mModelIsLoaded = false;
    private boolean mIsTargetCurrentlyTracked = false;
    
    private static final float OBJECT_SCALE_FLOAT = 0.003f;
    
    ImageTargetRenderer(ImageTargets activity, SampleApplicationSession session) {
        Log.v(LOGTAG,"fun: ImageTargetRenderer");
        mActivityRef = new WeakReference<>(activity);
        vuforiaAppSession = session;

        // SampleAppRenderer used to encapsulate the use of RenderingPrimitives setting
        // the device mode AR/VR and stereo mode
        mSampleAppRenderer = new SampleAppRenderer(this, mActivityRef.get(), vuforiaAppSession.getVideoMode(), 0.01f , 5f);
    }

    void updateRenderingPrimitives() {
        mSampleAppRenderer.updateRenderingPrimitives();
    }

    void setActive() {
        mSampleAppRenderer.setActive(true);
    }

    // The render function.
    // This function is called from the SampleAppRenderer by using the RenderingPrimitives views.
    // The state is owned by SampleAppRenderer which is controlling its lifecycle.
    // NOTE: State should not be cached outside this method.
    public void renderFrame(State state, float[] projectionMatrix) {
        Log.v(LOGTAG,"fun: renderFrame");
        // Renders video background replacing Renderer.DrawVideoBackground()
        mSampleAppRenderer.renderVideoBackground();

        // Set the device pose matrix as identity
        Matrix44F devicePoseMatrix = SampleMath.Matrix44FIdentity();
        Matrix44F modelMatrix;

        GLES20.glEnable(GLES20.GL_DEPTH_TEST);
        GLES20.glEnable(GLES20.GL_DEPTH_TEST);
        GLES20.glEnable(GLES20.GL_BLEND);
        GLES20.glCullFace(GLES20.GL_BACK);
        GLES20.glFrontFace(GLES20.GL_CCW);   // Back camera
        GLES20.glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Read device pose from the state and create a corresponding view matrix (inverse of the device pose)
        if (state.getDeviceTrackableResult() != null) {
            int statusInfo = state.getDeviceTrackableResult().getStatusInfo();
            int trackerStatus = state.getDeviceTrackableResult().getStatus();

            mActivityRef.get().checkForRelocalization(statusInfo);

            if (trackerStatus != TrackableResult.STATUS.NO_POSE) {
                modelMatrix = Tool.convertPose2GLMatrix(state.getDeviceTrackableResult().getPose());

                // We transpose here because Matrix44FInverse returns a transposed matrix
                devicePoseMatrix = SampleMath.Matrix44FTranspose(SampleMath.Matrix44FInverse(modelMatrix));
            }
        }

        TrackableResultList trackableResultList = state.getTrackableResults();

        // Determine if target is currently being tracked
        setIsTargetCurrentlyTracked(trackableResultList);

        // Iterate through trackable results and render any augmentations
        for (TrackableResult result : trackableResultList) {
            Trackable trackable = result.getTrackable();

            if (result.isOfType(ImageTargetResult.getClassType()) && result.getStatus() != TrackableResult.STATUS.LIMITED) {
                int textureIndex;
                modelMatrix = Tool.convertPose2GLMatrix(result.getPose());

                textureIndex = -1;

                if (trackable.getName().equalsIgnoreCase("reloj1") ||
                        trackable.getName().equalsIgnoreCase("reloj2") ||
                        trackable.getName().equalsIgnoreCase("reloj3"))
                    textureIndex = 0;
                if (trackable.getName().equalsIgnoreCase("paint1") ||
                        trackable.getName().equalsIgnoreCase("paint2") ||
                        trackable.getName().equalsIgnoreCase("paint3") ||
                        trackable.getName().equalsIgnoreCase("paint4") ||
                        trackable.getName().equalsIgnoreCase("paint5"))
                    textureIndex = 1;
                if (trackable.getName().equalsIgnoreCase("solar1") ||
                        trackable.getName().equalsIgnoreCase("solar2"))
                    textureIndex = 2;
                if (trackable.getName().equalsIgnoreCase("tens1") ||
                        trackable.getName().equalsIgnoreCase("tens2") ||
                        trackable.getName().equalsIgnoreCase("tens3") ||
                        trackable.getName().equalsIgnoreCase("tens4") ||
                        trackable.getName().equalsIgnoreCase("tens5") ||
                        trackable.getName().equalsIgnoreCase("tens6"))
                    textureIndex = 3;
                if (trackable.getName().equalsIgnoreCase("bear1") ||
                        trackable.getName().equalsIgnoreCase("bear2"))
                    textureIndex = 4;
                if (trackable.getName().equalsIgnoreCase("brazuca1") ||
                        trackable.getName().equalsIgnoreCase("brazuca2") ||
                        trackable.getName().equalsIgnoreCase("brazuca3") ||
                        trackable.getName().equalsIgnoreCase("brazuca4") ||
                        trackable.getName().equalsIgnoreCase("brazuca5") ||
                        trackable.getName().equalsIgnoreCase("brazuca6") ||
                        trackable.getName().equalsIgnoreCase("brazuca7") ||
                        trackable.getName().equalsIgnoreCase("brazuca8") ||
                        trackable.getName().equalsIgnoreCase("brazuca9") ||
                        trackable.getName().equalsIgnoreCase("brazuca10") ||
                        trackable.getName().equalsIgnoreCase("brazuca11") ||
                        trackable.getName().equalsIgnoreCase("brazuca12") ||
                        trackable.getName().equalsIgnoreCase("brazuca13"))
                    textureIndex = 5;
                if (trackable.getName().equalsIgnoreCase("abuelo"))
                    textureIndex = 6;
                if (trackable.getName().equalsIgnoreCase("abuela"))
                    textureIndex = 7;

                if (textureIndex > -1)
                    renderModel(projectionMatrix, devicePoseMatrix.getData(), modelMatrix.getData(), textureIndex);

                SampleUtils.checkGLError("Image Targets renderFrame");
            }
        }

        GLES20.glDisable(GLES20.GL_DEPTH_TEST);
    }

    @Override
    public void initRendering() {
        if (mTextures == null)
            return;
        Log.v(LOGTAG, "fun: initRendering");
        GLES20.glClearColor(0.0f, 0.0f, 0.0f, Vuforia.requiresAlpha() ? 0.0f : 1.0f);

        for (Texture t : mTextures) {
            GLES20.glGenTextures(1, t.mTextureID, 0);
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, t.mTextureID[0]);
            GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
            GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);
            GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_RGBA, t.mWidth, t.mHeight, 0, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE, t.mData);
        }

        shaderProgramID = SampleUtils.createProgramFromShaderSrc(CubeShaders.CUBE_MESH_VERTEX_SHADER,
                                                                CubeShaders.CUBE_MESH_FRAGMENT_SHADER);

        vertexHandle = GLES20.glGetAttribLocation(shaderProgramID, "vertexPosition");
        textureCoordHandle = GLES20.glGetAttribLocation(shaderProgramID, "vertexTexCoord");
        mvpMatrixHandle = GLES20.glGetUniformLocation(shaderProgramID, "modelViewProjectionMatrix");
        texSampler2DHandle = GLES20.glGetUniformLocation(shaderProgramID, "texSampler2D");

        if(!mModelIsLoaded) {
            Log.v(LOGTAG, "mModelIsLoaded: false");
            textClock = new TextClock();

            try {
                SampleApplication3DModel mBuildingsModel = new SampleApplication3DModel();
                mBuildingsModel.loadModel(mActivityRef.get().getResources().getAssets(), "ImageTargets/Buildings.txt");
                mModelIsLoaded = true;
                Log.v(LOGTAG, "mModelIsLoaded: true");
            } catch (IOException e) {
                Log.e(LOGTAG, "Unable to load buildings");
            }

            // Hide the Loading Dialog
            mActivityRef.get().loadingDialogHandler.sendEmptyMessage(LoadingDialogHandler.HIDE_LOADING_DIALOG);
        }
    }

    private void renderModel(float[] projectionMatrix, float[] viewMatrix, float[] modelMatrix, int textureIndex) {
        MeshObject model;
        float[] modelViewProjection = new float[16];

        // Apply local transformation to our model
        Matrix.translateM(modelMatrix, 0, 0, 0, OBJECT_SCALE_FLOAT);
        Matrix.scaleM(modelMatrix, 0, OBJECT_SCALE_FLOAT, OBJECT_SCALE_FLOAT, OBJECT_SCALE_FLOAT);
        Log.v(LOGTAG, "mTeapot");

        model = textClock;

        // Combine device pose (view matrix) with model matrix
        Matrix.multiplyMM(modelMatrix, 0, viewMatrix, 0, modelMatrix, 0);

        // Do the final combination with the projection matrix
        Matrix.multiplyMM(modelViewProjection, 0, projectionMatrix, 0, modelMatrix, 0);

        // Activate the shader program and bind the vertex and tex coords
        GLES20.glUseProgram(shaderProgramID);

        GLES20.glVertexAttribPointer(vertexHandle, 3, GLES20.GL_FLOAT, false, 0, model.getVertices());
        GLES20.glVertexAttribPointer(textureCoordHandle, 2, GLES20.GL_FLOAT, false, 0, model.getTexCoords());

        GLES20.glEnableVertexAttribArray(vertexHandle);
        GLES20.glEnableVertexAttribArray(textureCoordHandle);

        // Activate texture 0, bind it, pass to shader
        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mTextures.get(textureIndex).mTextureID[0]);
        GLES20.glUniform1i(texSampler2DHandle, 0);

        // Pass the model view matrix to the shader
        GLES20.glUniformMatrix4fv(mvpMatrixHandle, 1, false, modelViewProjection, 0);

        // Finally draw the model
        Log.v(LOGTAG, "isDeviceTrackingActive: false");
        GLES20.glDrawElements(GLES20.GL_TRIANGLES, model.getNumObjectIndex(), GLES20.GL_UNSIGNED_SHORT, model.getIndices());

        // Disable the enabled arrays
        GLES20.glDisableVertexAttribArray(vertexHandle);
        GLES20.glDisableVertexAttribArray(textureCoordHandle);
    }

    void setTextures(Vector<Texture> textures) {
        mTextures = textures;
    }

    private void setIsTargetCurrentlyTracked(TrackableResultList trackableResultList) {
        Log.v(LOGTAG, "fun: setIsTargetCurrentlyTracked");
        for(TrackableResult result : trackableResultList) {
            // Check the tracking status for result types
            // other than DeviceTrackableResult. ie: ImageTargetResult
            if (!result.isOfType(DeviceTrackableResult.getClassType())) {
                int currentStatus = result.getStatus();
                int currentStatusInfo = result.getStatusInfo();

                // The target is currently being tracked if the status is TRACKED|NORMAL
                if (currentStatus == TrackableResult.STATUS.TRACKED || currentStatusInfo == TrackableResult.STATUS_INFO.NORMAL) {
                    mIsTargetCurrentlyTracked = true;
                    return;
                }
            }
        }

        mIsTargetCurrentlyTracked = false;
    }

    boolean isTargetCurrentlyTracked() {
        return mIsTargetCurrentlyTracked;
    }
}
