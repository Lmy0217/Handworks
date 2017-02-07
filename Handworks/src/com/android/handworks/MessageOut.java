package com.android.handworks;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.util.Log;

public class MessageOut {
	private boolean msgIsSending = false;
	//private final String TAG = "SoundPlay";
	

	public static int msgMinBufferSize = AudioTrack.getMinBufferSize(48000,
																	 AudioFormat.CHANNEL_OUT_STEREO,
																	 AudioFormat.ENCODING_PCM_8BIT)*5;

	AudioTrack msgAT;
	
	public boolean msgIsSending(){
		return msgIsSending;
	}
	
	public void msgStart(byte[] carrierSignal) {
		if(msgIsSending)
			msgStop();
		
		msgAT = new AudioTrack(AudioManager.STREAM_MUSIC,
				48000,  
				AudioFormat.CHANNEL_OUT_MONO,
				AudioFormat.ENCODING_PCM_8BIT, 
				msgMinBufferSize,
				AudioTrack.MODE_STATIC);	
		Log.d("getMsgSamplerate", "" + 48000);
		
		msgIsSending = true;
		
		msgAT.write(carrierSignal, 0, EncoderCore.getEncoderBufferSize());
		Log.d("getEncoderBufferSize", "" + EncoderCore.getEncoderBufferSize());
		msgAT.flush();
		msgAT.setStereoVolume(0, 1);
		//msgAT.setLoopPoints(0, EncoderCore.getEncoderBufferSize(), -1);  
		msgAT.play();
	}

	public void msgStop() {
		if(msgAT != null)
		{
			msgAT.release();
			msgAT = null;
		}
		msgIsSending = false;
	}
}
