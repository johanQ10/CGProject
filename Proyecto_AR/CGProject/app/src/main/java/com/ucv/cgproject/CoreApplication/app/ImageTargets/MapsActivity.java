package com.ucv.cgproject.CoreApplication.app.ImageTargets;

import androidx.annotation.NonNull;
import androidx.core.content.ContextCompat;
import androidx.fragment.app.FragmentActivity;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.util.Log;
import android.view.View;

import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.MarkerOptions;
import com.ucv.cgproject.CoreApplication.PermissionUtils;
import com.ucv.cgproject.R;

public class MapsActivity extends FragmentActivity implements OnMapReadyCallback {
    private static final int LOCATION_PERMISSION_REQUEST_CODE = 1;
    private GoogleMap mMap;
    private String name;
    private String lon;
    private String lat;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_maps);

        if (getIntent().getExtras() != null) {
            name = getIntent().getExtras().getString("name");
            lon = getIntent().getExtras().getString("lon");
            lat = getIntent().getExtras().getString("lat");
        }

        findViewById(R.id.image_arrow).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                String mClassToLaunchPackage = getPackageName();
                String mClassToLaunch = mClassToLaunchPackage + ".CoreApplication.app.ImageTargets.ImageTargets";

                Intent i = new Intent();
                i.setClassName(getPackageName(), mClassToLaunch);
                startActivity(i);
                finish();
            }
        });

        // Obtain the SupportMapFragment and get notified when the map is ready to be used.
        SupportMapFragment mapFragment = (SupportMapFragment) getSupportFragmentManager().findFragmentById(R.id.map);
        mapFragment.getMapAsync(this);
    }

    @Override
    public void onMapReady(GoogleMap googleMap) {
        mMap = googleMap;

        // Add a marker in Sydney and move the camera
        LatLng ciens = new LatLng(Double.parseDouble(getString(R.string.location_lon)),
                Double.parseDouble(getString(R.string.location_lat)));
        mMap.addMarker(new MarkerOptions().position(ciens).title(getString(R.string.location)));

        LatLng loc0 = new LatLng(Double.parseDouble(getString(R.string.location_0_lon)),
                Double.parseDouble(getString(R.string.location_0_lat)));
        mMap.addMarker(new MarkerOptions().position(loc0).title(getString(R.string.location_0)));

        LatLng loc1 = new LatLng(Double.parseDouble(getString(R.string.location_1_lon)),
                Double.parseDouble(getString(R.string.location_1_lat)));
        mMap.addMarker(new MarkerOptions().position(loc1).title(getString(R.string.location_1)));

        LatLng loc2 = new LatLng(Double.parseDouble(getString(R.string.location_2_lon)),
                Double.parseDouble(getString(R.string.location_2_lat)));
        mMap.addMarker(new MarkerOptions().position(loc2).title(getString(R.string.location_2)));

        LatLng loc3 = new LatLng(Double.parseDouble(getString(R.string.location_3_lon)),
                Double.parseDouble(getString(R.string.location_3_lat)));
        mMap.addMarker(new MarkerOptions().position(loc3).title(getString(R.string.location_3)));

        LatLng loc4 = new LatLng(Double.parseDouble(getString(R.string.location_4_lon)),
                Double.parseDouble(getString(R.string.location_4_lat)));
        mMap.addMarker(new MarkerOptions().position(loc4).title(getString(R.string.location_4)));

        LatLng loc5 = new LatLng(Double.parseDouble(getString(R.string.location_5_lon)),
                Double.parseDouble(getString(R.string.location_5_lat)));
        mMap.addMarker(new MarkerOptions().position(loc5).title(getString(R.string.location_5)));

        LatLng loc6 = new LatLng(Double.parseDouble(getString(R.string.location_6_lon)),
                Double.parseDouble(getString(R.string.location_6_lat)));
        mMap.addMarker(new MarkerOptions().position(loc6).title(getString(R.string.location_6)));

        LatLng loc7 = new LatLng(Double.parseDouble(getString(R.string.location_7_lon)),
                Double.parseDouble(getString(R.string.location_7_lat)));
        mMap.addMarker(new MarkerOptions().position(loc7).title(getString(R.string.location_7)));

        LatLng result = new LatLng(Double.parseDouble(lon), Double.parseDouble(lat));
        mMap.moveCamera(CameraUpdateFactory.newLatLngZoom(result, 20));

        enableMyLocation();
    }

    private void enableMyLocation() {
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION)
                != PackageManager.PERMISSION_GRANTED) {
            // Permission to access the location is missing.
            PermissionUtils.requestPermission(this, LOCATION_PERMISSION_REQUEST_CODE,
                    Manifest.permission.ACCESS_FINE_LOCATION, true);
        } else if (mMap != null) {
            // Access to the location has been granted to the app.
            mMap.setMyLocationEnabled(true);
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        if (requestCode != LOCATION_PERMISSION_REQUEST_CODE) return;
        if (PermissionUtils.isPermissionGranted(permissions, grantResults,
                Manifest.permission.ACCESS_FINE_LOCATION)) enableMyLocation();
    }
}
