package com.example.newproject.api;

import com.example.newproject.object.Currency;
import com.example.newproject.object.CurrencyPost;
import com.example.newproject.object.ObjectPost;
import com.google.gson.Gson;
import com.google.gson.GsonBuilder;

import retrofit2.Call;
import retrofit2.Retrofit;
import retrofit2.converter.gson.GsonConverterFactory;
import retrofit2.http.Body;
import retrofit2.http.Header;
import retrofit2.http.POST;
import retrofit2.http.Query;

public interface ServerPostAPI {
    Gson gson = new GsonBuilder()
            .setDateFormat("yyyy-MM-dd HH:mm:ss")
            .create();

    ServerPostAPI servicePostAPI = new Retrofit.Builder()
            .baseUrl("http://api.innoway.vn/")
            .addConverterFactory(GsonConverterFactory.create(gson))
            .build()
            .create(ServerPostAPI.class);
    @POST("api/login")
    Call<CurrencyPost> getToken(@Body ObjectPost post);
}
