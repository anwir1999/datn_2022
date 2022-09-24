package com.example.newproject.fragment;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.fragment.app.Fragment;

import android.Manifest;
import android.content.pm.PackageManager;
import android.graphics.Point;
import android.location.Location;
import android.net.Uri;
import android.os.Bundle;
import android.os.Looper;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageButton;
import android.widget.Toast;

import com.android.volley.DefaultRetryPolicy;
import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.RetryPolicy;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.JsonObjectRequest;
import com.android.volley.toolbox.Volley;
import com.example.newproject.MainActivity;
import com.example.newproject.R;
import com.example.newproject.direct.FetchURL;
import com.example.newproject.direct.TaskLoadedCallback;
import com.google.android.gms.location.FusedLocationProviderClient;
import com.google.android.gms.location.LocationCallback;
import com.google.android.gms.location.LocationRequest;
import com.google.android.gms.location.LocationResult;
import com.google.android.gms.location.LocationServices;
import com.google.android.gms.location.Priority;
import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.LatLngBounds;
import com.google.android.gms.maps.model.Marker;
import com.google.android.gms.maps.model.MarkerOptions;
import com.google.android.gms.maps.model.Polyline;
import com.google.android.gms.maps.model.PolylineOptions;
import com.google.android.gms.tasks.OnSuccessListener;
import com.google.android.gms.tasks.Task;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

public class MapsFragment extends Fragment implements TaskLoadedCallback {

    private GoogleMap map;
    private double latht, lonht;
    private View view;
    private ImageButton imageButton;
    private MainActivity mainActivity;
    private Marker markerUser;
    private Marker markerHT;
    private MarkerOptions locationHt, locationUser;
    private LatLng latLng;
    public double latUser;
    public double lonUser;
    private Polyline currentPolyline;
    private String positionHt, positionUser;
    private FusedLocationProviderClient fusedLocationProviderClient;
    private PolylineOptions polylineOptions;
    private Polyline polyline;

    private OnMapReadyCallback callback = new OnMapReadyCallback() {

        /**
         * Manipulates the map once available.
         * This callback is triggered when the map is ready to be used.
         * This is where we can add markers or lines, add listeners or move the camera.
         * In this case, we just add a marker near Sydney, Australia.
         * If Google Play services is not installed on the device, the user will be prompted to
         * install it inside the SupportMapFragment. This method will only be triggered once the
         * user has installed Google Play services and returned to the app.
         */
        @Override
        public void onMapReady(GoogleMap googleMap) {
            map = googleMap;
            getHTLocation();
            getCurrentLocation();
            directionLocation();
        }
    };

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater,
                             @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        view = inflater.inflate(R.layout.fragment_maps, container, false);
        imageButton = view.findViewById(R.id.location);
        mainActivity = (MainActivity) getActivity();
        polylineOptions = null;
        imageButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(polyline != null)
                {
                    polyline.remove();
                }
                positionHt = "" + mainActivity.getLonht() + "," + mainActivity.getLatht();
                //positionHt = "" + 105.907063 + "," + 21.051809;
                positionUser = positionHt + ";" + lonUser + "," + latUser;
                direction();
                LatLngBounds bounds = new LatLngBounds.Builder()
                        .include(new LatLng(latht, lonht))
                        .include(new LatLng(latUser, lonUser)).build();
                Point point = mainActivity.getPoint();
                map.animateCamera(CameraUpdateFactory.newLatLngBounds(bounds, point.x, 150, 30));
                Toast.makeText(mainActivity.getApplicationContext(), "Test"
                            , Toast.LENGTH_SHORT).show();

            }
        });
        return view;
    }

    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        fusedLocationProviderClient =
                LocationServices.getFusedLocationProviderClient(mainActivity.getApplicationContext());
        SupportMapFragment mapFragment =
                (SupportMapFragment) getChildFragmentManager().findFragmentById(R.id.myMap);
        if (mapFragment != null) {
            mapFragment.getMapAsync(callback);
        }
    }

    private void getHTLocation() {
        if ((latht == 0 && lonht == 0) || mainActivity.getSt_check().equals("off") ) {
            if(markerHT == null)
            {
                return;
            }
            else {
                markerHT.remove();
                return;
            }
        } else {
            //LatLng latLng1 = new LatLng(latht, lonht);
            LatLng latLng1 = new LatLng(latht, lonht);
            locationHt = new MarkerOptions().position(latLng1).title("test location");
            markerHT = map.addMarker(locationHt);
//            map.moveCamera(CameraUpdateFactory.newLatLng(latLng1));
//            map.moveCamera(CameraUpdateFactory.newLatLngZoom(latLng1, 10));
        }
    }

    public void getUserLocation() {
        latLng = new LatLng(latUser, lonUser);
        locationUser = new MarkerOptions().position(latLng).title("current location");
        markerUser = map.addMarker(locationUser);
//        map.moveCamera(CameraUpdateFactory.newLatLng(latLng));
//        map.moveCamera(CameraUpdateFactory.newLatLngZoom(latLng, 15));
    }

    public void getCurrentLocation() {
        if (ActivityCompat.checkSelfPermission(mainActivity,
                Manifest.permission.ACCESS_FINE_LOCATION)
                != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(mainActivity,
                    new String[]{Manifest.permission.ACCESS_FINE_LOCATION,
                            Manifest.permission.ACCESS_FINE_LOCATION},
                    1);
        } else if (map != null) {
            map.setMyLocationEnabled(true);
            map.getUiSettings().setMyLocationButtonEnabled(true);
        }
        LocationRequest mLocationRequest = LocationRequest.create();
        mLocationRequest.setInterval(15000);
        mLocationRequest.setPriority(Priority.PRIORITY_HIGH_ACCURACY);
        mLocationRequest.setFastestInterval(500);
        LocationCallback locationCallback = new LocationCallback() {
            @Override
            public void onLocationResult(@NonNull LocationResult locationResult) {
                if (locationResult != null) {
//                    Toast.makeText(mainActivity.getApplicationContext(), "" + locationResult
//                            , Toast.LENGTH_SHORT).show();
                    latUser = locationResult.getLastLocation().getLatitude();
                    lonUser = locationResult.getLastLocation().getLongitude();
                    if(mainActivity.getSt_check().equals("on")) {
                        mainActivity.callServiceAPI();
                        latht = Double.parseDouble(mainActivity.getLatht());
                        lonht = Double.parseDouble(mainActivity.getLonht());
                    }
                    if (markerHT != null) {
                        markerHT.remove();
                    }
                    getHTLocation();
                    if (markerUser != null) {
                        markerUser.remove();
                    }
                    getUserLocation();
                    if( mainActivity.getSt_check().equals("off"))
                    {
                        if(polyline != null) {
                            polyline.remove();
                        }
                    }
                    if(polylineOptions != null)
                    {
                        directionLocation();
                    }
                    Log.i("gpsabc", "" + lonUser);
                    Log.i("gpsabc", "" + latUser);
                } else {
                    Toast.makeText(mainActivity.getApplicationContext(), "Current location is null", Toast.LENGTH_LONG).show();
                    return;
                }
                for (Location location : locationResult.getLocations()) {
                    if (locationResult == null) {
                        Toast.makeText(mainActivity.getApplicationContext(), "Current location is null", Toast.LENGTH_LONG).show();
                    }
                }
            }
        };
        if (ActivityCompat.checkSelfPermission(mainActivity, Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED && ActivityCompat.checkSelfPermission(mainActivity, Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
            // TODO: Consider calling
            //    ActivityCompat#requestPermissions
            // here to request the missing permissions, and then overriding
            //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
            //                                          int[] grantResults)
            // to handle the case where the user grants the permission. See the documentation
            // for ActivityCompat#requestPermissions for more details.
            return;
        }
        fusedLocationProviderClient.requestLocationUpdates(mLocationRequest, locationCallback
                , Looper.myLooper());
        Task<Location> task = fusedLocationProviderClient.getLastLocation();
        task.addOnSuccessListener(new OnSuccessListener<Location>() {
            @Override
            public void onSuccess(Location location) {
                if(location != null)
                {
                }
            }
        });
    }

    private String getUrl(LatLng origin, LatLng dest, String directionMode) {
        // Origin of route
        String str_origin = "origin=" + origin.latitude + "," + origin.longitude;
        // Destination of route
        String str_dest = "destination=" + dest.latitude + "," + dest.longitude;
        // Mode
        String mode = "mode=" + directionMode;
        // Building the parameters to the web service
        String parameters = str_origin + "&" + str_dest + "&" + mode;
        // Output format
        String output = "json";
        // Building the url to the web service
        String url = "https://maps.googleapis.com/maps/api/directions/" + output + "?" + parameters + "&key=" + getString(R.string.map_key);
        return url;
    }
    @Override
    public void onTaskDone(Object... values) {
        if (currentPolyline != null)
            currentPolyline.remove();
        currentPolyline = map.addPolyline((PolylineOptions) values[0]);
    }
    private void direction(){
        RequestQueue requestQueue = Volley.newRequestQueue(mainActivity);
        String url = Uri.parse("https://router.project-osrm.org/route/v1/driving/"+ positionUser)
                .buildUpon()
                .appendQueryParameter("steps", "true")
                .appendQueryParameter("alternatives", "true")
//                .appendQueryParameter("mode", "driving")
//                .appendQueryParameter("key", getString(R.string.map_key))
                .toString();
        JsonObjectRequest jsonObjectRequest = new JsonObjectRequest(Request.Method.GET, url, null, new Response.Listener<JSONObject>() {
            @Override
            public void onResponse(JSONObject response) {
                try {
                    String status = response.getString("code");
                    if (status.equals("Ok")) {
                        Toast.makeText(mainActivity, "Test2"
                                , Toast.LENGTH_SHORT).show();
                        JSONArray routes = response.getJSONArray("routes");

                        ArrayList<LatLng> points;

                        for (int i=0;i<routes.length();i++){
                            points = new ArrayList<>();
                            polylineOptions = new PolylineOptions();
                            JSONArray legs = routes.getJSONObject(i).getJSONArray("legs");

                            for (int j=0;j<legs.length();j++){
                                JSONArray steps = legs.getJSONObject(j).getJSONArray("steps");

                                for (int k=0;k<steps.length();k++){
                                    JSONArray intersections = steps.getJSONObject(k).getJSONArray("intersections");
                                    for(int c=0;c<intersections.length();c++)
                                    {
                                        JSONArray location = intersections.getJSONObject(c).getJSONArray("location");
//                                        List<LatLng> list = decodePoly(polyline);
                                        LatLng position = new LatLng(location.getDouble(1), location.getDouble(0));
                                        points.add(position);
                                    }
                                    //String polyline = steps.getJSONObject(k).getJSONObject("polyline").getString("points");
//                                    List<LatLng> list = decodePoly(polyline);

                                    //for (int l=0;l<list.size();l++){
                                    //    LatLng position = new LatLng((list.get(l)).latitude, (list.get(l)).longitude);
                                    //
                                    //}
                                }
                            }
                            polylineOptions.addAll(points);
                            polylineOptions.width(10);
                            polylineOptions.color(ContextCompat.getColor(mainActivity, R.color.purple_500));
                            polylineOptions.geodesic(true);
                        }
                    }
                } catch (JSONException e) {
                    e.printStackTrace();
                }
            }
        }, new Response.ErrorListener() {
            @Override
            public void onErrorResponse(VolleyError error) {
                Toast.makeText(mainActivity, "Test"
                        , Toast.LENGTH_SHORT).show();
            }
        });
        RetryPolicy retryPolicy = new DefaultRetryPolicy(30000, DefaultRetryPolicy.DEFAULT_MAX_RETRIES, DefaultRetryPolicy.DEFAULT_BACKOFF_MULT);
        jsonObjectRequest.setRetryPolicy(retryPolicy);
        requestQueue.add(jsonObjectRequest);
    }
    private List<LatLng> decodePoly(String encoded){
        List<LatLng> poly = new ArrayList<>();
        int index = 0, len = encoded.length();
        int lat = 0, lng = 0;

        while (index < len) {
            int b, shift = 0, result = 0;
            do {
                b = encoded.charAt(index++) - 63;
                result |= (b & 0x1f) << shift;
                shift += 5;
            } while (b >= 0x20);
            int dlat = ((result & 1) != 0 ? ~(result >> 1) : (result >> 1));
            lat += dlat;

            shift = 0;
            result = 0;
            do {
                b = encoded.charAt(index++) - 63;
                result |= (b & 0x1f) << shift;
                shift += 5;
            } while (b > 0x20);
            int dlng = ((result & 1) != 0 ? ~(result >> 1) : (result >> 1));
            lng += dlng;

            LatLng p = new LatLng((((double) lat / 1E5)),
                    (((double) lng / 1E5)));
            poly.add(p);
        }
        return poly;
    }
    private void directionLocation()
    {
        if(polylineOptions != null && mainActivity.getSt_check().equals("on")) {
            if(polyline != null) {
                polyline.remove();
            }
            polyline = map.addPolyline(polylineOptions);
        }
    }
}