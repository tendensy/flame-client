package com.weiktonplaceint.game.launcher;

import static com.weiktonplaceint.game.core.Config.GAME_PATH;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;
import retrofit2.Retrofit;
import retrofit2.converter.gson.GsonConverterFactory;

import com.weiktonplaceint.game.network.ApiService;
import com.weiktonplaceint.game.network.Links;

import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

import com.weiktonplaceint.game.R;

import java.io.File;
import java.util.Timer;
import java.util.TimerTask;

public class SplashActivity extends AppCompatActivity
{
    private TextView textloading;

    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.splash_activity);
        textloading = (TextView) findViewById(R.id.textView4);
        if (Build.VERSION.SDK_INT >= 23) {
            if (checkSelfPermission(Manifest.permission.READ_EXTERNAL_STORAGE) == PackageManager.PERMISSION_DENIED
                    || checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_DENIED) {
                requestPermissions(new String[]{Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE}, 1000);
            } else {
                startTimer();
            }
        } else {
            startTimer();
        }
    }
    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == 1000) {
            startTimer();
        }
    }

    private void loadAPI() {
        ApiService.getInstance().getApiService().getLinks().enqueue(new Callback<Links>() {
            public void onResponse(Call<Links> call, Response<Links> response) {
                if(response.isSuccessful()) {
                    try {
                        if (response.body() != null) {
                            ApiService.getInstance().ClientVersion = response.body().getTargetClientVersion();
                            ApiService.getInstance().GameFilesVersion = response.body().getTargetGameFilesVersion();
                            ApiService.getInstance().URL_GAME_FILES = response.body().getUrlFiles();
                            ApiService.getInstance().URL_CLIENT = response.body().getUrlClient();
                        }
                    } catch (Exception e) {

                    }
                }
            }
            public void onFailure(Call<Links> call, Throwable th) {
                Toast.makeText(getApplicationContext(), "Сервера подгрузки не доступны", Toast.LENGTH_SHORT).show();
            }
        });
    }

    private boolean IsGameInstalled()
    {
        String GetGamePath = GAME_PATH + "texdb/gta3.img";
        File file = new File(GetGamePath);
        return file.exists();
    }

    private void startTimer()
    {
        Timer t = new Timer();
        t.schedule(new TimerTask(){

            @Override
            public void run() {
                startText();
            }
        }, 1000L);
    }

    private void startText()
    {
        textloading.setText("Соединение с PLACEINT..");
        Timer t = new Timer();
        t.schedule(new TimerTask(){

            @Override
            public void run() {
                startText2();
                loadAPI();
            }
        }, 7000L);
    }
    private void startText2()
    {
        textloading.setText("Настройка конфигурации..");
        Timer t = new Timer();
        t.schedule(new TimerTask(){

            @Override
            public void run() {
                startText3();
            }
        }, 1000L);
    }
    private void startText3()
    {
        textloading.setText("Добро пожаловать!");
        Timer t = new Timer();
        t.schedule(new TimerTask(){

            @Override
            public void run() {
                startLauncher();
            }
        }, 2000L);
    }

    private void startLauncher()
    {
        if(IsGameInstalled()) {
            startActivity(new Intent(this, com.weiktonplaceint.game.launcher.MainActivity.class));
            finish();
        } else {
            textloading.setText("Проверка файлов..");
            //showMessage("Установите нужные файлы");
            startActivity(new Intent(this, com.weiktonplaceint.game.launcher.DownloadActivity.class));
            finish();
        }
    }
}
