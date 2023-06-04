package com.weiktonplaceint.game.core;

import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.appcompat.widget.AppCompatButton;
import androidx.fragment.app.DialogFragment;
import androidx.viewpager.widget.ViewPager;

import com.weiktonplaceint.game.R;
import com.google.android.material.tabs.TabLayout;
import com.nvidia.devtech.NvEventQueueActivity;


public class DialogClientSettings extends DialogFragment {
    TabLayout tabLayout;
    ViewPager viewPager;

    static final int mSettingsHudCount = 10;
    static final int mSettingsHudFPSStart = 10;
    static final int mSettingsHudFPSEnd = 12;

    static final int mSettingsWeaponsStart = 12;
    static final int mSettingsWeaponsEnd = 14;

    static final int mSettingsComonStart = 14;
    static final int mSettingsComonEnd = 15;
    NvEventQueueActivity mContext = null;
}
