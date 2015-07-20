package com.example.androidtest;

import android.app.Activity;
import android.content.Context;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.util.Log;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Arrays;

public class TestActivity extends Activity{
    static {
        System.loadLibrary("testlibrary");
    }

    @Override
    protected void onCreate(Bundle savedInstance){
        super.onCreate(savedInstance);
        Log.i("TestActivity", "Launching new thread to run the request");
        String dirPath=getApplicationContext().getApplicationInfo().dataDir;

        Log.i("TestActivity", "Dirpath:"+dirPath);
        // saveCertPemFile();
        setDir(saveCertPemFile());


        new Thread(){
            @Override
            public void run(){
                String url = "https://en.wikipedia.org/wiki/Main_Page";
                Log.i("TestActivity", "Requesting URL to download: " + url);

                byte[] content = downloadUrl(url);
                String contentString=content == null ? null : new String(content);
                Log.i("TestActivity", contentString != null ?
                        (contentString.length() > 50 ?
                                "First 50 bytes downloaded: " + contentString.substring(0,50)
                                : "Downloaded data: " + contentString)
                                : "Content is null");
            }
        }.start();

    }

    public native byte[] downloadUrl(String url);

    public native void setDir(String caCertDir);


    private String saveCertPemFile()
    {
        Context context=getApplicationContext();
        String assetFileName="cacert.pem";

        if(context==null || !FileExistInAssets(assetFileName,context))
        {
            Log.i("TestActivity", "Context is null or asset file doesnt exist");
            return null;
        }
        //destination path is data/data/packagename
        String destPath=getApplicationContext().getApplicationInfo().dataDir;
        String CertFilePath =destPath + "/" +assetFileName;
        File file = new File(CertFilePath);
        if(file.exists())
        {
            //delete file
            file.delete();
        }
        //copy to internal storage
        if(CopyAssets(context,assetFileName,CertFilePath)==1) return CertFilePath;

        return CertFilePath=null;

    }

    private int CopyAssets(Context context,String assetFileName, String toPath)
    {
        AssetManager assetManager = context.getAssets();
        InputStream in = null;
        OutputStream out = null;
        try {
            in = assetManager.open(assetFileName);
            new File(toPath).createNewFile();
            out = new FileOutputStream(toPath);
            byte[] buffer = new byte[1024];
            int read;
            while ((read = in.read(buffer)) != -1)
            {
                out.write(buffer, 0, read);
            }
            in.close();
            in = null;
            out.flush();
            out.close();
            out = null;
            return 1;
        } catch(Exception e) {
            Log.e("tag", "CopyAssets"+e.getMessage());

        }
        return 0;

    }

    private boolean FileExistInAssets(String fileName,Context context)
    {
        try {
            return Arrays.asList(context.getResources().getAssets().list("")).contains(fileName);
        } catch (IOException e) {
            // TODO Auto-generated catch block

            Log.e("tag", "FileExistInAssets"+e.getMessage());

        }
        return false;
    }


};
