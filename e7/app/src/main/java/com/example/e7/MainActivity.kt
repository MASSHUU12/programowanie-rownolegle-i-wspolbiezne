package com.example.e7

import android.os.Bundle
import android.view.View
import androidx.appcompat.app.AppCompatActivity
import androidx.lifecycle.lifecycleScope
import com.example.e7.databinding.ActivityMainBinding
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
    }

    private external fun calculate(matrixSize: Int, cpus: Int): String

    fun onCalculateClick(view: View) {
        val cpus = binding.editCpus.text.toString().toIntOrNull() ?: return
        val matrixSize = binding.editMatrixSize.text.toString().toIntOrNull() ?: return

        binding.resultText.text = ""
        binding.calculateButton.isClickable = false
        binding.workIndicator.visibility = View.VISIBLE

        Thread {
            val result = calculate(matrixSize, cpus)
            runOnUiThread {
                binding.resultText.text = result
                binding.calculateButton.isClickable = true
                binding.workIndicator.visibility = View.INVISIBLE
            }
        }.start()
    }

    companion object {
        // Used to load the 'e7' library on application startup.
        init {
            System.loadLibrary("e7")
        }
    }
}
