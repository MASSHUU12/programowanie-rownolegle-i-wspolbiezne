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

    private external fun calculate(matrixSize: Int, cpus: Int): String

    fun onCalculateAll(view: View) {
        val cpus = getCpus() ?: return
        val matrixSize = getMatrixSize() ?: return

        runOnUiThread {
            setupBeforeCalculating()
        }

        Thread {
            val cppResult = calculate(matrixSize, cpus)
            // TODO: Calculate Kotlin
            val ktResult = ""

            runOnUiThread {
                cleanupAfterCalculating(cppResult + ktResult)
            }
        }.start()
    }

    fun onCalculateCpp(view: View) {
        val cpus = getCpus() ?: return
        val matrixSize = getMatrixSize() ?: return

        runOnUiThread {
            setupBeforeCalculating()
        }

        Thread {
            val result = calculate(matrixSize, cpus)
            runOnUiThread {
                cleanupAfterCalculating(result)
            }
        }.start()
    }

    fun onCalculateKt(view: View) {
        val cpus = getCpus() ?: return
        val matrixSize = getMatrixSize() ?: return

        runOnUiThread {
            setupBeforeCalculating()
        }

        Thread {
            // TODO
            val result = ""
            runOnUiThread {
                cleanupAfterCalculating(result)
            }
        }.start()
    }

    private fun getCpus(): Int? {
        return binding.editCpus.text.toString().toIntOrNull()
    }

    private fun getMatrixSize(): Int? {
        return binding.editMatrixSize.text.toString().toIntOrNull()
    }

    private fun setupBeforeCalculating() {
        setOutputText("")
        toggleLoading(true)
        toggleButtons(false)
    }

    private fun cleanupAfterCalculating(text: String) {
        setOutputText(
            "Matrix size: " + getMatrixSize() + "\nCPUs: " + getCpus() + "\n\n" + text
        )
        toggleLoading(false)
        toggleButtons(true)
    }

    private fun toggleButtons(clickable: Boolean) {
        binding.calculateKtButton.isClickable = clickable
        binding.calculateAllButton.isClickable = clickable
        binding.calculateCppButton.isClickable = clickable
    }

    private fun toggleLoading(loading: Boolean) {
        binding.workIndicator.visibility = if (loading) View.VISIBLE else View.INVISIBLE
    }

    private fun setOutputText(text: String) {
        binding.resultText.text = text
    }

    companion object {
        // Used to load the 'e7' library on application startup.
        init {
            System.loadLibrary("e7")
        }
    }
}
