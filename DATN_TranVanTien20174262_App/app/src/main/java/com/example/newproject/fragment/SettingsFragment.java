package com.example.newproject.fragment;

import android.os.Bundle;

import androidx.appcompat.widget.SwitchCompat;
import androidx.fragment.app.Fragment;

import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.Spinner;
import android.widget.Toast;

import com.example.newproject.MainActivity;
import com.example.newproject.R;
import com.example.newproject.dropSpinner.Category;
import com.example.newproject.dropSpinner.CatergoryAdapter;

import java.util.ArrayList;
import java.util.List;

/**
 * A simple {@link Fragment} subclass.
 * Use the {@link SettingsFragment#newInstance} factory method to
 * create an instance of this fragment.
 */
public class SettingsFragment extends Fragment {

    // TODO: Rename parameter arguments, choose names that match
    // the fragment initialization parameters, e.g. ARG_ITEM_NUMBER
    private static final String ARG_PARAM1 = "param1";
    private static final String ARG_PARAM2 = "param2";

    // TODO: Rename and change types of parameters
    private String mParam1;
    private String mParam2;
    private Spinner spinnerCate;
    private CatergoryAdapter catergoryAdapter;
    private MainActivity mainActivity;
    private int tpDevice;
    private SwitchCompat switchCompat;
    private Button button;
    private String stspk;
    public SettingsFragment() {
        // Required empty public constructor
    }

    /**
     * Use this factory method to create a new instance of
     * this fragment using the provided parameters.
     *
     * @param param1 Parameter 1.
     * @param param2 Parameter 2.
     * @return A new instance of fragment SettingsFragment.
     */
    // TODO: Rename and change types and number of parameters
    public static SettingsFragment newInstance(String param1, String param2) {
        SettingsFragment fragment = new SettingsFragment();
        Bundle args = new Bundle();
        args.putString(ARG_PARAM1, param1);
        args.putString(ARG_PARAM2, param2);
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (getArguments() != null) {
            mParam1 = getArguments().getString(ARG_PARAM1);
            mParam2 = getArguments().getString(ARG_PARAM2);
        }
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View view = inflater.inflate(R.layout.fragment_settings, container, false);
        spinnerCate = view.findViewById(R.id.spinner_cate);
        switchCompat = view.findViewById(R.id.bt_switch);
        button = view.findViewById(R.id.bt_apply);

        mainActivity = (MainActivity) getActivity();
        catergoryAdapter = new CatergoryAdapter(mainActivity,R.layout.item_selected, getListCate());
        spinnerCate.setAdapter(catergoryAdapter);
        spinnerCate.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                if(catergoryAdapter.getItem(position).getTimePeriod().equals("2 minutes"))
                {
                    tpDevice = 120;
                }
                else if(catergoryAdapter.getItem(position).getTimePeriod().equals("5 minutes"))
                {
                    tpDevice = 300;
                }
                else if(catergoryAdapter.getItem(position).getTimePeriod().equals("10 minutes"))
                {
                    tpDevice = 600;
                }
                else if(catergoryAdapter.getItem(position).getTimePeriod().equals("10 seconds"))
                {
                    tpDevice = 10;
                }
                else if(catergoryAdapter.getItem(position).getTimePeriod().equals("30 seconds"))
                {
                    tpDevice = 30;
                }
                else if(catergoryAdapter.getItem(position).getTimePeriod().equals("1 minute"))
                {
                    tpDevice = 60;
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {

            }
        });
        switchCompat.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                stspk = switchCompat.isChecked() ? "on" : "off";
            }
        });
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(mainActivity.getSt_check().equals("off"))
                {
                    Toast.makeText(mainActivity, "State of device is off", Toast.LENGTH_SHORT).show();
                }
                else if(mainActivity.getSt_check().equals("on"))
                {
                    mainActivity.publishMessageHT("{\"stCheck\":\"on\",\"stpk\":\""+ stspk + "\",\"tp\":\"" + tpDevice +"\"}");
                    mainActivity.publishMessageAtributes("{\"stCheck\":\"on\",\"stpk\":\""+ stspk + "\",\"tp\":\"" + tpDevice +"\"}");
                }
            }
        });
        return view;
    }
    private List<Category> getListCate() {
        List<Category> list = new ArrayList<>();
        list.add(new Category("10 seconds"));
        list.add(new Category("30 seconds"));
        list.add(new Category("1 minute"));
        list.add(new Category("2 minutes"));
        list.add(new Category("2 minutes"));
        list.add(new Category("5 minutes"));
        list.add(new Category("10 minutes"));
        return list;
    }
}