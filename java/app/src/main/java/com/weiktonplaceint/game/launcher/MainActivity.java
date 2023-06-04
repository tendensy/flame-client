package com.weiktonplaceint.game.launcher;

import android.content.Intent;
import android.os.Bundle;
import android.os.Environment;
import android.view.KeyEvent;
import android.view.View;
import android.view.inputmethod.EditorInfo;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import com.weiktonplaceint.game.BuildConfig;
import com.weiktonplaceint.game.R;
import com.weiktonplaceint.game.core.Utils;
import com.weiktonplaceint.game.gui.ScreenActivity;
import com.weiktonplaceint.game.network.ApiService;

import androidx.appcompat.app.AppCompatActivity;
import org.ini4j.Wini;
import java.io.File;
import java.io.IOException;

import static com.weiktonplaceint.game.core.Config.GAME_PATH;

public class MainActivity extends AppCompatActivity
{
    private Button btn_play;
    private EditText nickname;

    protected void onCreate(Bundle savedInstanceState)
    {
        setContentView(R.layout.main_activity);
        Init(savedInstanceState);
        super.onCreate(savedInstanceState);
    }

    public void onClickPlay() {
        File gameFiles = new File(Environment.getExternalStorageDirectory() + "/RealRussia/SAMP/settings.ini");
        Integer TARGET_GAMEFILES_VERSION = 0;
        if (gameFiles.exists()) {
            try {
                Wini w = new Wini(gameFiles);
                TARGET_GAMEFILES_VERSION = Integer.valueOf(w.get("versions", "gameFilesVersion"));
                w.store();
            } catch (IOException e) {

            }
        }

        if(ApiService.getInstance().ClientVersion != BuildConfig.VERSION_CODE){
            //startActivity(new Intent(this, DownloadActivity.class));
            finish();
        }
        else if (ApiService.getInstance().GameFilesVersion != TARGET_GAMEFILES_VERSION) {
            File file = new File(Environment.getExternalStorageDirectory() + "/ReallRussia");
            if (file.exists()) {
                file.delete();
            }
            startActivity(new Intent(this, DownloadActivity.class));
            finish();
        }
        else if(IsGameInstalled()) {
            startActivity(new Intent(getApplicationContext(), com.weiktonplaceint.game.core.GTASA.class));
        } else {
            startActivity(new Intent(getApplicationContext(), DownloadActivity.class));
        }
    }

    private boolean IsGameInstalled()
    {
        String GetGamePath = GAME_PATH + "texdb/gta3.img";
        File file = new File(GetGamePath);
        return file.exists();
    }

    private void Init(Bundle savedInstanceState)
    {
        btn_play = (Button) findViewById(R.id.btn_play);
        nickname = (EditText) findViewById(R.id.nick_edit);
        btn_play.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View _view) {
                //onClickPlay();
                StartClient2();
                //if(IsGameInstalled()) StartClient();
                //else StartInstallGame();
            }
        });
        ((EditText)nickname).setOnEditorActionListener(new EditText.OnEditorActionListener() {
            @Override
            public boolean onEditorAction(TextView v, int actionId, KeyEvent event)
            {
                if (actionId == EditorInfo.IME_ACTION_SEARCH || actionId == EditorInfo.IME_ACTION_DONE || event.getAction() == KeyEvent.ACTION_DOWN && event.getKeyCode() == KeyEvent.KEYCODE_ENTER)
                {
                    try {
                        File f = new File(GAME_PATH+"SAMP/crmp_settings.ini");
                        if(!f.exists()){ f.createNewFile();f.mkdirs();}
                        Wini w = new Wini(new File(GAME_PATH+"SAMP/crmp_settings.ini"));
                        w.put("client", "name", nickname.getText().toString());
                        w.store();
                        showMessage("Ваш новый никнейм успешно сохранен!");
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
                return false;
            }
        });
        InitLogic();
    }

    private void InitLogic() {
        try{
            Wini w = new Wini(new File(GAME_PATH+"SAMP/crmp_settings.ini"));
            nickname.setText(w.get("client", "name"));
            w.store();
            //CheckUpdateLauncher();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void StartInstallGame()
    {
        Intent intent = new Intent(this, com.weiktonplaceint.game.launcher.DownloadActivity.class);
        Utils.setType(0);
        intent.putExtras(getIntent());
        startActivity(intent);
    }

    private void StartClient2()
    {
        Intent intent = new Intent(this, ScreenActivity.class);
        intent.putExtras(getIntent());
        System.out.println("StartActivity GTASA.class");
        startActivity(intent);
        finish();
    }

    private void StartClient()
    {
        Intent intent = new Intent(this, com.weiktonplaceint.game.core.GTASA.class);
        intent.putExtras(getIntent());
        System.out.println("StartActivity GTASA.class");
        startActivity(intent);
        finish();
    }

    private void showMessage(String _s) {
        Toast.makeText(getApplicationContext(), _s, Toast.LENGTH_SHORT).show();
    }
}
