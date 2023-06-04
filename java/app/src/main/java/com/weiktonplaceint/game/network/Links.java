package com.weiktonplaceint.game.network;

import com.google.gson.annotations.SerializedName;
import com.weiktonplaceint.game.BuildConfig;

public class Links {
    @SerializedName("clientVersionCode")
    private int ClientVersion = BuildConfig.VERSION_CODE;

    @SerializedName("gameFilesVersionCode")
    private Integer GameFilesVersion;

    @SerializedName("URL_GAME_FILES")
    private String URL_GAME_FILES;

    @SerializedName("URL_CLIENT")
    private String URL_CLIENT;
    
    public Links() { }
    public final int getTargetClientVersion() {
        return ClientVersion;
    }
    public final Integer getTargetGameFilesVersion() {
        return GameFilesVersion;
    }
    public final String getUrlFiles() {
        return URL_GAME_FILES;
    }
    public final String getUrlClient() {
        return URL_CLIENT;
    }
}
