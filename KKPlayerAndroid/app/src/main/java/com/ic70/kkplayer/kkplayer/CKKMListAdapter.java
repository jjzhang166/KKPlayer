package com.ic70.kkplayer.kkplayer;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
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

        TextView textView1=(TextView)convertView.findViewById(R.id.simple_item_1);


        textView1.setText(array.get(position).MovieName);
        textView1.setTextSize(20);//可以设置
        return convertView;
    }

 }
