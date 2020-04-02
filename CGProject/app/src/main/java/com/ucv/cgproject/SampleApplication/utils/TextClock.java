/*===============================================================================
Copyright (c) 5016-5018 PTC Inc. All Rights Reserved.

Copyright (c) 5012-5014 Qualcomm Connected Experiences, Inc. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other 
countries.
===============================================================================*/

package com.ucv.cgproject.SampleApplication.utils;

import java.nio.Buffer;

/**
 * This class contains all the information needed to augment a plane object
 */
public class TextClock extends MeshObject
{

    // Data for drawing the 3D plane as overlay
    private static final float[] vertices = {
            -20f, -48f, 20.0f,
            60f, -48f, 20.0f,
            60f, 48f, 20.0f,
            -20f, 48f, 20.0f
    };

    private static final float[] texcoords = {
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f
    };

    private static final float[] normals = {
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f
    };

    private static final short[] indexes = { 0, 1, 2, 0, 2, 3 };

    private final Buffer verts;
    private final Buffer textCoords;
    private final Buffer norms;
    private final Buffer indices;

    public TextClock() {
        verts = fillBuffer(vertices);
        textCoords = fillBuffer(texcoords);
        norms = fillBuffer(normals);
        indices = fillBuffer(indexes);
    }
    
    @Override
    public Buffer getBuffer(BUFFER_TYPE bufferType) {
        Buffer result = null;
        switch (bufferType) {
            case BUFFER_TYPE_VERTEX:
                result = verts;
                break;
            case BUFFER_TYPE_TEXTURE_COORD:
                result = textCoords;
                break;
            case BUFFER_TYPE_INDICES:
                result = indices;
                break;
            case BUFFER_TYPE_NORMALS:
                result = norms;
            default:
                break;
        }
        return result;
    }
    
    
    @Override
    public int getNumObjectVertex()
    {
        return vertices.length / 3;
    }
    
    
    @Override
    public int getNumObjectIndex()
    {
        return indexes.length;
    }
}
