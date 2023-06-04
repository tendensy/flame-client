package com.weiktonplaceint.game.launcher;

import android.content.Intent;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.View;
import android.widget.ProgressBar;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

import com.weiktonplaceint.game.R;

import com.weiktonplaceint.game.network.ApiService;
import net.lingala.zip4j.core.ZipFile;
import net.lingala.zip4j.exception.ZipException;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.net.URLConnection;

public class DownloadActivity extends AppCompatActivity {
    private final String link = String.valueOf(ApiService.getInstance().URL_GAME_FILES);
    public final String path_zip = (Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOWNLOADS) + "/cache.zip");
    ProgressBar progressBar;
    ProgressBar progressBarInstall;
    TextView textView6;
    TextView textView7;
    TextView textview5;

    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView((int) R.layout.activity_install);
        this.textview5 = (TextView) findViewById(R.id.textView5555);
        this.textView7 = (TextView) findViewById(R.id.textView7777);
        this.textView6 = (TextView) findViewById(R.id.textView6666);
        this.progressBar = (ProgressBar) findViewById(R.id.progressBarnew0);
        File file = new File(Environment.getExternalStorageDirectory() + "/BlackRussia");
        if (file.exists()) {
            file.delete();
        }
        this.progressBarInstall = (ProgressBar) findViewById(R.id.progressBarInstallda0);
        try {
            getFileSize(String.valueOf(ApiService.getInstance().URL_GAME_FILES));
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void getFileSize(final String string) throws IOException {
        new Thread(new Runnable() {
            FileOutputStream file = new FileOutputStream(DownloadActivity.this.path_zip);

            InputStream f115in;
            URLConnection urlConnection = null;

            public void run() {
                try {
                    URLConnection openConnection = new URL(string).openConnection();
                    this.urlConnection = openConnection;
                    openConnection.connect();
                    int length = this.urlConnection.getContentLength();
                    this.f115in = this.urlConnection.getInputStream();
                    byte[] data = new byte[4096];
                    int chet = 0;
                    while (true) {
                        int read = this.f115in.read(data, 0, 4096);
                        int count = read;
                        if (read == -1) {
                            break;
                        }
                        chet += count;
                        this.file.write(data, 0, count);
                        int finalChet = chet;
                        DownloadActivity.this.runOnUiThread(new Runnable() {
                            public final void run() {
                                DownloadActivity.this.lambda$run$0$InstallActivity$1(finalChet, length);
                            }
                        });
                    }
                    FileOutputStream fileOutputStream = this.file;
                    if (fileOutputStream != null) {
                        try {
                            fileOutputStream.close();
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                    }
                    InputStream inputStream = this.f115in;
                    if (inputStream != null) {
                        try {
                            inputStream.close();
                        } catch (IOException e2) {
                            e2.printStackTrace();
                        }
                    }
                } catch (Exception e3) {
                    Log.e("Download", "Ошибка " + e3);
                    FileOutputStream fileOutputStream2 = this.file;
                    if (fileOutputStream2 != null) {
                        try {
                            fileOutputStream2.close();
                        } catch (IOException e4) {
                            e4.printStackTrace();
                        }
                    }
                    InputStream inputStream2 = this.f115in;
                    if (inputStream2 != null) {
                        try {
                            inputStream2.close();
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                    }
                } catch (Throwable th) {
                    FileOutputStream fileOutputStream3 = this.file;
                    if (fileOutputStream3 != null) {
                        try {
                            fileOutputStream3.close();
                        } catch (IOException e5) {
                            e5.printStackTrace();
                        }
                    }
                    InputStream inputStream3 = this.f115in;
                    if (inputStream3 != null) {
                        try {
                            inputStream3.close();
                        } catch (IOException e6) {
                            e6.printStackTrace();
                        }
                    }
                    throw th;
                }
                DownloadActivity.this.runOnUiThread(new Runnable() {
                    public final void run() {
                        DownloadActivity.this.lambda$run$1$InstallActivity$1();
                    }
                });
                DownloadActivity.this.UnZip();
            }
        }).start();
    }

    private void lambda$run$1$InstallActivity$1() {
        DownloadActivity.this.textview5.setText("Распаковка файлов игры...");
        DownloadActivity.this.textView6.setVisibility(View.INVISIBLE);
        DownloadActivity.this.textView7.setVisibility(View.INVISIBLE);
        DownloadActivity.this.progressBarInstall.setVisibility(View.INVISIBLE);
        DownloadActivity.this.progressBar.setVisibility(View.VISIBLE);
    }

    private void lambda$run$0$InstallActivity$1(int finalChet, int length) {
        TextView textView = DownloadActivity.this.textView7;
        textView.setText((finalChet / 1048576) + " MB из " + (length / 1048576) + " MB");
        TextView textView2 = DownloadActivity.this.textView6;
        StringBuilder sb = new StringBuilder();
        sb.append(((finalChet / 1048576) * 100) / (length / 1048576));
        sb.append("%");
        textView2.setText(sb.toString());
        DownloadActivity.this.progressBarInstall.setProgress(((finalChet / 1048576) * 100) / (length / 1048576));
    }

    public void UnZip() {
        try {
            new ZipFile(this.path_zip).extractAll(String.valueOf(Environment.getExternalStorageDirectory()));
            new File(this.path_zip).delete();
        } catch (ZipException e) {
            e.printStackTrace();
        }
        startActivity(new Intent(this, com.weiktonplaceint.game.launcher.MainActivity.class));
        overridePendingTransition(0, 0);
    }

    public static String getExtractCmd(String str, String str2) {
        return String.format("7z x '%s' '-o%s' -aoa", new Object[]{ str, str2 });
    }

    public static String getCompressCmd(String str, String str2, String str3) {
        return String.format("7z a -t%s '%s' '%s'", new Object[]{ str3, str2, str });
    }
}