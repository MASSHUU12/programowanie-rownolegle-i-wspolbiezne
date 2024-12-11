package com.example.e7

import android.os.Bundle
import android.view.View
import androidx.appcompat.app.AppCompatActivity
import com.example.e7.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
    }

    private external fun calculate(matrixSize: Int, blockSize: Int, cpus: Int): String

    fun onCalculateClick(view: View) {
        val matrixSize = binding.editMatrixSize.text.toString().toIntOrNull() ?: return
        val blockSize = binding.editBlockSize.text.toString().toIntOrNull() ?: return
        val cpus = binding.editCpus.text.toString().toIntOrNull() ?: return

        binding.resultText.text = ""
        binding.workIndicator.visibility = View.VISIBLE

        view.post {
            // TODO: Move to a separate thread to keep the main thread responsive
            binding.resultText.text = calculate(matrixSize, blockSize, cpus)
            binding.workIndicator.visibility = View.INVISIBLE
        }
    }

    companion object {
        // Used to load the 'e7' library on application startup.
        init {
            System.loadLibrary("e7")
        }
    }
}
