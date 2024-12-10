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

    private external fun calculate(matrixSize: Int, blockSize: Int): String

    fun onCalculateClick(view: View) {
        val matrixSize = binding.editMatrixSize.text.toString().toIntOrNull() ?: return
        val blockSize = binding.editBlockSize.text.toString().toIntOrNull() ?: return

        binding.resultText.text = calculate(matrixSize, blockSize)
    }

    companion object {
        // Used to load the 'e7' library on application startup.
        init {
            System.loadLibrary("e7")
        }
    }
}
