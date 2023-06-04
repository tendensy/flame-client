package com.weiktonplaceint.game.network;

import retrofit2.Retrofit;
import retrofit2.converter.gson.GsonConverterFactory;

public class ApiService {
    private static ApiService instance;
    private Retrofit retrofit = new Retrofit.Builder().baseUrl("https://brussia-new.reactnet.site/").addConverterFactory(GsonConverterFactory.create()).build();

    public int ClientVersion;
    public Integer GameFilesVersion;
    public String URL_GAME_FILES;
    public String URL_CLIENT;

    private ApiService() {
        /* code */
    }

    public static ApiService getInstance() {
        if (instance == null) {
            instance = new ApiService();
        }
        return instance;
    }

    public Api getApiService() {
        return this.retrofit.create(Api.class);
    }
}
