package com.example.e7

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.View
import android.widget.TextView
import com.example.e7.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
    }

    private external fun calculate(): String

    fun onCalculateClick(view: View) {
        binding.resultText.text = calculate()
    }

    companion object {
        // Used to load the 'e7' library on application startup.
        init {
            System.loadLibrary("e7")
        }
    }
}
