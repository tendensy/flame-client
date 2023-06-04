package com.weiktonplaceint.game.network;

import retrofit2.Call;
import retrofit2.http.GET;

public interface Api {

    String apulink = "https://api-blackmoscow.glitch.me/api.json";

    @GET(apulink)
    Call<Links> getLinks();
}
