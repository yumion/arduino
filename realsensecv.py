# coding: utf-8
import pyrealsense2 as rs
import numpy as np
import cv2


class RealsenseCapture:

    def __init__(self):
        self.WIDTH = 640
        self.HEGIHT = 480
        self.FPS = 30
        # Configure depth and color streams
        self.config = rs.config()
        # Pipeline streaming
        self.pipeline = rs.pipeline()

    def start(self):
        # Config
        self.config.enable_stream(rs.stream.color, self.WIDTH, self.HEGIHT, rs.format.bgr8, self.FPS)
        self.config.enable_stream(rs.stream.depth, self.WIDTH, self.HEGIHT, rs.format.z16, self.FPS)
        # Align the angle of view
        self.align = rs.align(rs.stream.color)
        # Start streaming
        self.pipeline.start(self.config)
        print('pipeline start')

    def read(self, is_filtered=False, raw_frame=False):
        # Flag capture available
        ret = True
        # get frames
        frames = self.pipeline.wait_for_frames()
        aligned_frames = self.align.process(frames)  # Align the depth frame to color frame
        # separate RGB and Depth image
        self.color_frame = aligned_frames.get_color_frame()  # RGB
        self.depth_frame = aligned_frames.get_depth_frame()  # Depth

        if not self.color_frame or not self.depth_frame:
            ret = False
            return ret, (None, None)

        if raw_frame:
            return ret, (self.color_frame, self.depth_frame)

        else:
            # Get filtered depth frame
            depth_frame = self.filtering(self.depth_frame) if is_filtered else self.depth_frame
            # Express depth frame by heatmap
            depth_colorized_frame = rs.colorizer().colorize(depth_frame)
            # Convert images to numpy arrays
            color_image = np.array(self.color_frame.get_data())
            depth_image = np.array(depth_colorized_frame.get_data())
            # depth_colorized_image = cv2.applyColorMap(cv2.convertScaleAbs(depth_image, alpha=0.08), cv2.COLORMAP_JET)
            return ret, (color_image, depth_image)

    def release(self):
        # Stop streaming
        self.pipeline.stop()
        print('pipeline stopped')

    def filtering(self, frame):
        '''Filter setting'''
        # Decimation #
        decimation = rs.decimation_filter()
        decimation.set_option(rs.option.filter_magnitude, 1)
        # Spatial #
        spatial = rs.spatial_filter()
        # spatial.set_option(rs.option.filter_magnitude, 5)
        spatial.set_option(rs.option.filter_smooth_alpha, 0.6)
        spatial.set_option(rs.option.filter_smooth_delta, 8)
        # spatial.set_option(rs.option.holes_fill, 3)
        # Temporal #
        temporal = rs.temporal_filter()
        temporal.set_option(rs.option.filter_smooth_alpha, 0.5)
        temporal.set_option(rs.option.filter_smooth_delta, 20)
        # Hole #
        hole_filling = rs.hole_filling_filter()
        ##
        depth_to_disparity = rs.disparity_transform(True)
        disparity_to_depth = rs.disparity_transform(False)

        '''Appling filter'''
        frame = decimation.process(frame)
        frame = depth_to_disparity.process(frame)
        frame = spatial.process(frame)
        frame = temporal.process(frame)
        frame = disparity_to_depth.process(frame)
        frame = hole_filling.process(frame)
        return frame
