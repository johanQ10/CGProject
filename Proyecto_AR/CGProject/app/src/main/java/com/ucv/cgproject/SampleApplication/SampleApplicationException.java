/*===============================================================================
Copyright (c) 2016-2018 PTC Inc. All Rights Reserved.

Copyright (c) 2012-2014 Qualcomm Connected Experiences, Inc. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other 
countries.
===============================================================================*/

package com.ucv.cgproject.SampleApplication;

/**
 * This class is used to send any Vuforia Engine initialization errors back to the activity
 */
public class SampleApplicationException extends Exception
{
    private static final long serialVersionUID = 2L;

    // Error codes
    static final int INITIALIZATION_FAILURE = 0;
    static final int VUFORIA_ALREADY_INITIALIZATED = 1;
    static final int TRACKERS_INITIALIZATION_FAILURE = 2;
    static final int LOADING_TRACKERS_FAILURE = 3;
    static final int UNLOADING_TRACKERS_FAILURE = 4;
    static final int TRACKERS_DEINITIALIZATION_FAILURE = 5;
    static final int CAMERA_INITIALIZATION_FAILURE = 6;
    
    private final int mCode;
    private final String mString;
    
    
    SampleApplicationException(int code, String description)
    {
        super(description);
        mCode = code;
        mString = description;
    }
    
    
    public int getCode()
    {
        return mCode;
    }
    
    
    public String getString()
    {
        return mString;
    }
}
