package com.example.newproject.api;

import android.annotation.SuppressLint;

import com.example.newproject.MainActivity;
import com.example.newproject.object.Currency;
import com.google.android.material.internal.ContextUtils;
import com.google.gson.Gson;
import com.google.gson.GsonBuilder;

import retrofit2.Call;
import retrofit2.Retrofit;
import retrofit2.converter.gson.GsonConverterFactory;
import retrofit2.http.GET;
import retrofit2.http.Header;
import retrofit2.http.Headers;
import retrofit2.http.Path;
import retrofit2.http.Query;

public interface ServiceCallAPI {

    //{{host}}/api/attributes/DEVICE/d829afac-2a21-46f4-af67-3adb93233a57/values?offset=0&limit=100007
    Gson gson = new GsonBuilder()
            .setDateFormat("yyyy-MM-dd HH:mm:ss")
            .create();

    ServiceCallAPI serviceAPI = new Retrofit.Builder()
            .baseUrl("http://api.innoway.vn/")
            .addConverterFactory(GsonConverterFactory.create(gson))
            .build()
            .create(ServiceCallAPI.class);

    @GET("api/attributes/DEVICE/bbaadd3d-0a6d-43b3-85db-db46381d2914/values")
    //@Headers("Authorization: Bearer {token}")
    Call<Currency> getData(@Header("Authorization") String Authorization, @Query("offset") int offset, @Query("limit") int limit);
}
