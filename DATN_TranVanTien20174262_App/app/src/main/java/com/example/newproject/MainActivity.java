package com.example.newproject;


import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.fragment.app.Fragment;
import androidx.fragment.app.FragmentManager;
import androidx.fragment.app.FragmentTransaction;
import androidx.viewpager2.widget.ViewPager2;

import android.Manifest;
import android.annotation.SuppressLint;
import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.Point;
import android.location.Location;
import android.location.LocationManager;
import android.os.Bundle;
import android.util.Log;
import android.view.MenuItem;
import android.widget.Toast;

import com.example.newproject.api.ServerPostAPI;
import com.example.newproject.api.ServiceCallAPI;
import com.example.newproject.fragment.HomeFragment;
import com.example.newproject.fragment.MapsFragment;
import com.example.newproject.fragment.ViewPGAdapter;
import com.example.newproject.object.Currency;
import com.example.newproject.object.CurrencyPost;
import com.example.newproject.object.ObjectPost;
import com.google.android.gms.location.FusedLocationProviderClient;
import com.google.android.gms.location.LocationCallback;
import com.google.android.gms.location.LocationRequest;
import com.google.android.gms.location.LocationResult;
import com.google.android.gms.location.LocationServices;
import com.google.android.gms.location.Priority;
import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.MarkerOptions;
import com.google.android.gms.tasks.OnSuccessListener;
import com.google.android.gms.tasks.Task;
import com.google.android.material.bottomnavigation.BottomNavigationView;
import com.google.android.material.navigation.NavigationBarView;

import org.eclipse.paho.android.service.MqttAndroidClient;
import org.eclipse.paho.client.mqttv3.IMqttActionListener;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.IMqttToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class MainActivity extends AppCompatActivity {

    private ViewPager2 viewPager2;
    private BottomNavigationView bottomNavigationView;
    private MapsFragment mapsFragment;
    private HomeFragment homeFragment;
    private long backPressedTime;
    private  Toast toast;
    // call api
    private String st_check;
    private String tp;
    private String authorization;
    // location
    private String latht;
    private String lonht;
    private Point point;
    private String timeht;
    private String dateht;

    // mqtt
    final private String TAG = "mqttabc";
    final private String PUB_TOPICAT = "messages/bbaadd3d-0a6d-43b3-85db-db46381d2914/attributets";
    final private String SUB_TOPIC = "messages/bbaadd3d-0a6d-43b3-85db-db46381d2914/ht32";
    final private String host = "tcp://116.101.122.190:1883";

    private MqttAndroidClient mqttAndroidClient;
    final private String userName = "d829afac-2a21-46f4-af67-3adb93233a57";
    final private String passWord = "R0BeRO26eUCd8vLM4R4GWGe5W2i6Jxj8";
    final private String clientId = "1c512ee8-1db3-4f00-b629-eb4ab4bca46d";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        //callServiceAPI();
        // anh xa
        viewPager2 = findViewById(R.id.view_pg2);
        bottomNavigationView = findViewById(R.id.bt_navi);
        FragmentManager fm =  getSupportFragmentManager();
        mapsFragment = (MapsFragment) fm.findFragmentByTag("MapsFragment");
        homeFragment = (HomeFragment) fm.findFragmentByTag("HomeFragment");
        point = new Point();
        getWindowManager().getDefaultDisplay().getSize(point);
//        fusedLocationProviderClient =
//                LocationServices.getFusedLocationProviderClient(this.getApplicationContext());

        ViewPGAdapter viewPGAdapter = new ViewPGAdapter(this);
        viewPager2.setAdapter(viewPGAdapter);
        //viewPager2.setPageTransformer(new DepthPageTransformer());
        viewPager2.setUserInputEnabled(false);
        viewPager2.registerOnPageChangeCallback(new ViewPager2.OnPageChangeCallback() {
            @Override
            public void onPageSelected(int position) {
                super.onPageSelected(position);
                switch (position) {
                    case 0:
                        bottomNavigationView.getMenu().findItem(R.id.menu_home).setChecked(true);
                        break;
                    case 1:
                        bottomNavigationView.getMenu().findItem(R.id.menu_map).setChecked(true);
                        break;
                    case 2:
                        bottomNavigationView.getMenu().findItem(R.id.menu_settings).setChecked(true);
                        break;
                }
            }
        });
        bottomNavigationView.setOnItemSelectedListener(new NavigationBarView.OnItemSelectedListener() {
            @Override
            public boolean onNavigationItemSelected(@NonNull MenuItem item) {
                switch (item.getItemId())
                {
                    case R.id.menu_home:
                        viewPager2.setCurrentItem(0);
                        break;
                    case R.id.menu_map:
                        viewPager2.setCurrentItem(1);
                        break;
                    case R.id.menu_settings:
                        viewPager2.setCurrentItem(2);
                        break;
                }
                return true;
            }
        });

        // mqtt
        /* Create an MqttConnectOptions object and configure the username and password. */
        MqttConnectOptions mqttConnectOptions = new MqttConnectOptions();
        mqttConnectOptions.setUserName(userName);
        mqttConnectOptions.setPassword(passWord.toCharArray());

        mqttAndroidClient = new MqttAndroidClient(this.getApplicationContext(), host, clientId);
        mqttAndroidClient.setCallback(new MqttCallback() {
            @Override
            public void connectionLost(Throwable cause) {
                Log.i(TAG, "connection lost");
            }

            @Override
            public void messageArrived(String topic, MqttMessage message) throws Exception {
                Log.i(TAG, "topic: " + topic + ", msg: " + new String(message.getPayload()));
            }

            @Override
            public void deliveryComplete(IMqttDeliveryToken token) {
                Log.i(TAG, "msg delivered");
            }
        });
        /* Establish a connection to IoT Platform by using MQTT. */
        try {
            mqttAndroidClient.connect(mqttConnectOptions, null, new IMqttActionListener() {
                @Override
                public void onSuccess(IMqttToken asyncActionToken) {
                    Log.i(TAG, "connect succeed");

                    //subscribeTopic(SUB_TOPIC);
                }
                @Override
                public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                    Log.i(TAG, "connect failed");
                }
            });

        } catch (MqttException e) {
            e.printStackTrace();
        }
    }
//    public void subscribeTopic(String topic) {
//        try {
//            mqttAndroidClient.subscribe(topic, 0, null, new IMqttActionListener() {
//                @Override
//                public void onSuccess(IMqttToken asyncActionToken) {
//                    Log.i(TAG, "subscribed succeed");
//                }
//                @Override
//                public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
//                    Log.i(TAG, "subscribed failed");
//                }
//            });
//
//        } catch (MqttException e) {
//            e.printStackTrace();
//        }
//    }
    public void publishMessageHT(String payload) {
        try {
            if (!mqttAndroidClient.isConnected()) {
                mqttAndroidClient.connect();
            }
            MqttMessage message = new MqttMessage();
            message.setPayload(payload.getBytes());
            message.setQos(0);
            message.setRetained(true);
            mqttAndroidClient.publish(SUB_TOPIC, message, null, new IMqttActionListener() {
                @Override
                public void onSuccess(IMqttToken asyncActionToken) {
                    Log.i(TAG, "publish succeed!");
                }
                @Override
                public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                    Log.i(TAG, "publish failed!");
                }
            });
        } catch (MqttException e) {
            Log.e(TAG, e.toString());
            e.printStackTrace();
        }
    }
    public void publishMessageAtributes(String payload) {
        try {
            if (!mqttAndroidClient.isConnected()) {
                mqttAndroidClient.connect();
            }
            MqttMessage messageAT = new MqttMessage();
            messageAT.setPayload(payload.getBytes());
            messageAT.setQos(0);
            messageAT.setRetained(true);
            mqttAndroidClient.publish(PUB_TOPICAT, messageAT, null, new IMqttActionListener() {
                @Override
                public void onSuccess(IMqttToken asyncActionToken) {
                    Log.i(TAG, "publish succeed!");
                }
                @Override
                public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                    Log.i(TAG, "publish failed!");
                }
            });
        } catch (MqttException e) {
            Log.e(TAG, e.toString());
            e.printStackTrace();
        }
    }
    public void callServiceAPI()
    {
        ServiceCallAPI.serviceAPI.getData(authorization,0, 10000).enqueue(new Callback<Currency>() {
            @Override
            public void onResponse(Call<Currency> call, Response<Currency> response) {
                //Toast.makeText(MainActivity.this, "Call API success", Toast.LENGTH_SHORT).show();
                if(response.body()!= null)
                {
                    for(int i = 0; i < response.body().getAttributes().size(); i++)
                    {
                        if(response.body().getAttributes().get(i).getAttribute_key().equals("stCheck"))
                        {
                            st_check = response.body().getAttributes().get(i).getValue();
                        }
                        else if(response.body().getAttributes().get(i).getAttribute_key().equals("tp"))
                        {
                            tp = response.body().getAttributes().get(i).getValue() + " s/m";
                        }
                        else if(response.body().getAttributes().get(i).getAttribute_key().equals("latht"))
                        {
                            latht = response.body().getAttributes().get(i).getValue() ;
                        }
                        else if(response.body().getAttributes().get(i).getAttribute_key().equals("lonht"))
                        {
                            lonht = response.body().getAttributes().get(i).getValue();
                        }
                        else if(response.body().getAttributes().get(i).getAttribute_key().equals("timeht"))
                        {
                            timeht = response.body().getAttributes().get(i).getValue();
                        }
                        else if(response.body().getAttributes().get(i).getAttribute_key().equals("dateht"))
                        {
                            dateht = response.body().getAttributes().get(i).getValue();
                        }
                    }
                }
            }

            @Override
            public void onFailure(Call<Currency> call, Throwable t) {
                Toast.makeText(MainActivity.this, "Call API failed", Toast.LENGTH_SHORT).show();
            }
        });
    }
    public void PostServiceAPI()
    {
        ObjectPost objectPost = new ObjectPost("tickervietnam@gmail.com", "anhtien1609");
        ServerPostAPI.servicePostAPI.getToken(objectPost).enqueue(new Callback<CurrencyPost>() {
            @Override
            public void onResponse(Call<CurrencyPost> call, Response<CurrencyPost> response) {
                if(response.body() != null)
                {
                    authorization = "Bearer " + response.body().getToken();
                    callServiceAPI();
                }
            }
            @Override
            public void onFailure(Call<CurrencyPost> call, Throwable t) {

            }
        });
    }
    @SuppressLint("MissingSuperCall")
    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {

        switch (requestCode)
        {
            case 101:
                if(grantResults.length>0 && grantResults[0] == PackageManager.PERMISSION_GRANTED)
                {
                    //getCurrentLocation();
                }
        }
    }

    @Override
    public void onBackPressed() {
        if(backPressedTime + 2000 > System.currentTimeMillis())
        {
            toast.cancel();
            super.onBackPressed();
        }
        else
        {
            toast = Toast.makeText(MainActivity.this, " Press back again to exit to application", Toast.LENGTH_SHORT);
            toast.show();
        }
        backPressedTime = System.currentTimeMillis();
    }

    public String getSt_check() {
        return st_check;
    }

    public void setSt_check(String st_check) {
        this.st_check = st_check;
    }

    public String getTp() {
        return tp;
    }

    public void setTp(String tp) {
        this.tp = tp;
    }

    public String getLatht() {
        return latht;
    }

    public String getLonht() {
        return lonht;
    }

    public String getAuthorization() {
        return authorization;
    }

    public void setAuthorization(String authorization) {
        this.authorization = authorization;
    }

    public Point getPoint() {
        return point;
    }

    public void setPoint(Point point) {
        this.point = point;
    }

    public String getTimeht() {
        return timeht;
    }

    public String getDateht() {
        return dateht;
    }
}