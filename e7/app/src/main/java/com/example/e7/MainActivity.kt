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

        setDefaultValues()
    }

    private external fun calculate(matrixSize: Int, cpus: Int): Double

    fun onCalculateAll(view: View) {
        val cpus = getCpus() ?: return
        val matrixSize = getMatrixSize() ?: return

        setupBeforeCalculating()

        lifecycleScope.launch {
            val cppResult = withContext(Dispatchers.Default) {
                calculate(matrixSize, cpus)
            }
            val ktResult = withContext(Dispatchers.Default) {
                MatrixMultiplier(matrixSize, cpus).calculate()
            }

            cleanupAfterCalculating(cppResult, ktResult)
        }
    }

    fun onCalculateCpp(view: View) {
        val cpus = getCpus() ?: return
        val matrixSize = getMatrixSize() ?: return

        setupBeforeCalculating()

        lifecycleScope.launch {
            val result = withContext(Dispatchers.Default) {
                calculate(matrixSize, cpus)
            }

            cleanupAfterCalculating(result, -1.0)
        }
    }

    fun onCalculateKt(view: View) {
        val cpus = getCpus() ?: return
        val matrixSize = getMatrixSize() ?: return

        setupBeforeCalculating()

        lifecycleScope.launch {
            val result = withContext(Dispatchers.Default) {
                MatrixMultiplier(matrixSize, cpus).calculate()
            }

            cleanupAfterCalculating(-1.0, result)
        }
    }

    private fun setDefaultValues() {
        binding.editMatrixSize.setText("1024")
        binding.editCpus.setText(getNumberOfPhysicalCores().toString())
    }

    private fun getNumberOfPhysicalCores(): Int {
        return Runtime.getRuntime().availableProcessors()
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

    private fun cleanupAfterCalculating(timeCpp: Double, timeKt: Double) {
        setOutputText(
            "Matrix size: " + getMatrixSize() + "\nCPUs: " + getCpus() + "\n\n"
                + "Multiplication time:\n"
                + (if (timeCpp > -1.0) "C++: " + timeCpp + "s\n" else "")
                + (if (timeKt > -1.0) "Kotlin: " + timeKt + "s\n" else "")
                + '\n'
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
