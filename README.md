# FaceTrack
FaceTrack: Asymmetric Facial and Gesture Analysis Tool for Speech Language Pathologist Applications

In this paper, a novel tool for facial and gesture analysis aiming to quantify different subjective measures employed in the Speech Language Pathologist area is proposed.
Through an input video (from a simple monocular camera) showing a person's face, the developed tool can track the movements and expressions from it in order to extract useful morphological and gestural parameters that are of interest in different fields of study, such as, Speech Language Pathologist, Neurology, etc.
A modified version of a $3D$ face model, named Candide-3, is employed in the tracking stage.
Since the original $3D$ model cannot handle asymmetrical facial movements, a new set of animation units was implemented in order to effectively track asymmetrical gestures.
To enhance the tracking accuracy, a fusion scheme is proposed in the facial gesture tracking stage by means of the combination of the $3D$ face model previously described and facial landmarks detected using deep learning models. 
This tool will be made open source, both the software application (oriented to health professionals, no need to have any programming knowledge), and the source code for the computer vision community.
Several perceptual experiments were carried out, achieving promising results.

## Citation
```
@inproceedings{Sad_Reyes_Alvarez_2021,
  title={FaceTrack: Asymmetric Facial and Gesture Analysis Tool for Speech Language Pathologist Applications},
  author={Sad, Gonzalo D. and Reyes, Facundo and Julián, Alvarez},
  booktitle={Proceedings of the 1st Workshop on Facial Micro-Expression: Advanced Techniques for Facial Expressions Generation and Spotting (FME'21)},
  year={2021}
}
```