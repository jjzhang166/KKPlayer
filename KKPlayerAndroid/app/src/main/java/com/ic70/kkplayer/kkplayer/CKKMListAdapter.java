package com.ic70.kkplayer.kkplayer;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.TextView;

import java.util.List;

/**
 * Created by saint on 2016/4/16.
 */

public class CKKMListAdapter extends BaseAdapter
{
    private Context context;
    private List<CKKMoviePath> array;

    CKKMListAdapter(Context context,List<CKKMoviePath> array){
        this.context=context;
        this.array=array;
    }
    public int getCount() {
        // TODO Auto-generated method stub
        return array.size();
    }

    @Override
    public Object getItem(int position) {
        // TODO Auto-generated method stub
        return array.get(position);
    }

    @Override
    public long getItemId(int position) {
        // TODO Auto-generated method stub
        return position;
    }
    public View getView(int position,View convertView,ViewGroup parent)
    {
        if(convertView==null){
            LayoutInflater inflater = (LayoutInflater)context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            convertView=inflater.inflate(R.layout.mlist_item, null);
        }
        CheckBox cb=( CheckBox)convertView.findViewById(R.id.CheckBox_0);
        cb.setOnCheckedChangeListener(new CKKCheckBoxClick(this));
        TextView textView1=(TextView)convertView.findViewById(R.id.simple_item_1);
        textView1.setText(array.get(position).MovieName);
        textView1.setTextSize(20);//可以设置
        return convertView;
    }
    protected void onCheckBoxClick(View view)
    {
        CheckBox cb=(CheckBox)view;
        Intent intent = new Intent();
        intent.putExtra("MoviePath","rtmp://live.hkstv.hk.lxdns.com/live/hks live=1");
        /* 指定intent要启动的类 */
        intent.setClass(context,CPlayerActivity.class);//(context.this, Activity02.class);
        /* 启动一个新的Activity */
        context.startActivity(intent);
        /* 关闭当前的Activity */
       // ((Activity)context).finish();
    }
    class CKKCheckBoxClick implements CompoundButton.OnCheckedChangeListener {

        CKKMListAdapter m_KKlist;
        public CKKCheckBoxClick(CKKMListAdapter KKlist) {
            m_KKlist =KKlist;
        }
        @Override
        public void onCheckedChanged(CompoundButton var1, boolean var2)
        {
            m_KKlist.onCheckBoxClick( var1);
        }

       /* public void onClick(View view) {
             switch(view.getId())
             {
                   case R.id.CheckBox_0:
                                            m_KKlist.onCheckBoxClick(view);
             }
        }*/
    }
 }
